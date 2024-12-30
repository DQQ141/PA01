/**
 * @file uart_transparent.cc
 * @author WittXie
 * @brief 透传串口驱动
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../bsp.h"
#include "./../protocol_bsp.h"

static void rf_update_receive_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    // 拦截接收的数据
    protocol_t *p_protocol_source = (protocol_t *)device;
    protocol_t *p_protocol_target = (protocol_t *)userdata;

    uint16_t length = ring_data_size(&p_protocol_source->ring);
    if (length != 0)
    {
        uint8_t *buff = os_malloc(length);
        ASSERT(buff != NULL);

        ring_dequeue(&p_protocol_source->ring, buff, length);
        p_protocol_target->ops.write(buff, length); // 转发

        if (memcmp(buff, "waiting", 7) == 0)
        {
            log_trace("[%s] received data[%u] [%s]", p_protocol_source->cfg.name, length, buff);
        }

        os_free(buff);
        p_protocol_source->recv_length = 0; // 重置接收长度
    }
}

static void rf_update_send_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    // 拦截发送的数据
    protocol_t *protocol = (protocol_t *)device;
    protocol->send_length = 0;
}

static struct
{
    bool is_update;
    bool is_exec;
} s_uart_transparent = {
    .is_update = false,
    .is_exec = false,
};

// RF模式设置
void rf_update_mode_set(bool is_update)
{
    s_uart_transparent.is_update = is_update;
    s_uart_transparent.is_exec = true;
}
bool rf_update_mode_get(void)
{
    return s_uart_transparent.is_update;
}

// RF模式设置
void rf_reboot(void)
{
    s_uart_transparent.is_exec = true;
}

static void transparent_entry(void *args)
{
    os_rely_wait(protocol_is_running(&g_protocol_uart_rf) == true, 6000);
    os_rely_wait(protocol_is_running(&g_protocol_uart_head) == true, 6000);
    os_rely_wait(gpio_is_running(&g_exout_rfpower_enable) == true, 6000);
    os_rely_wait(gpio_is_running(&g_exout_gpio0) == true, 6000);

    for (;;)
    {
        os_sleep(100);
        if (CMP_PREV(s_uart_transparent.is_update) || s_uart_transparent.is_exec)
        {
            s_uart_transparent.is_exec = false;
            if (s_uart_transparent.is_update)
            {
                // 订阅串口接收
                dds_subcribe(&g_protocol_uart_head.RAW_RECEIVE, DDS_PRIORITY_NORMAL, rf_update_receive_callback, &g_protocol_uart_rf); // 转发 到 rf
                dds_subcribe(&g_protocol_uart_rf.RAW_RECEIVE, DDS_PRIORITY_NORMAL, rf_update_receive_callback, &g_protocol_uart_head); // 转发 到 head

                dds_subcribe(&g_protocol_uart_head.RAW_SEND, DDS_PRIORITY_NORMAL, rf_update_send_callback, NULL);
                dds_subcribe(&g_protocol_uart_rf.RAW_SEND, DDS_PRIORITY_NORMAL, rf_update_send_callback, NULL);

                uart_abort(&UART_RF);
                uart_abort(&UART_HEAD);
                gpio_write(&g_exout_rfpower_enable, false); // 先关闭RF模块电源
                gpio_write(&g_exout_gpio0, true);           // 让RF模块进入烧录模式
                os_sleep(100);
                gpio_write(&g_exout_rfpower_enable, true); // 打开RF模块电源
                os_sleep(2000);                            // 等待RF模块启动后再设置波特率，因为RF模块启动后会自动发送一堆非正常波形,容易导致串口异常
                uart_baud_set(&UART_RF, 115200);           // 设置波特率
                uart_baud_set(&UART_HEAD, 115200);         // 设置波特率
                HAL_UART_Receive_DMA(&(UART_RF), uart_rf_read_buff, UART_RF_DMA_BUFF_SIZE);
                HAL_UART_Receive_DMA(&(UART_HEAD), uart_head_read_buff, UART_HEAD_DMA_BUFF_SIZE);
            }
            else
            {
                uart_abort(&UART_RF);
                uart_abort(&UART_HEAD);
                gpio_write(&g_exout_rfpower_enable, false); // 关闭RF模块电源
                gpio_write(&g_exout_gpio0, false);          // 让RF模块退出烧录模式
                os_sleep(100);
                gpio_write(&g_exout_rfpower_enable, true); // 重新打开RF模块电源
                os_sleep(2000);                            // 等待RF模块启动后再设置波特率，因为RF模块启动后会自动发送一堆非正常波形,容易导致串口异常
                uart_baud_set(&UART_RF, 1500000);          // 设置波特率
                uart_baud_set(&UART_HEAD, 115200);         // 设置波特率
                HAL_UART_Receive_DMA(&(UART_RF), uart_rf_read_buff, UART_RF_DMA_BUFF_SIZE);
                HAL_UART_Receive_DMA(&(UART_HEAD), uart_head_read_buff, UART_HEAD_DMA_BUFF_SIZE);

                // 取消订阅串口接收
                dds_unsubcribe(&g_protocol_uart_head.RAW_RECEIVE, rf_update_receive_callback);
                dds_unsubcribe(&g_protocol_uart_rf.RAW_RECEIVE, rf_update_receive_callback);
                dds_unsubcribe(&g_protocol_uart_head.RAW_SEND, rf_update_send_callback);
                dds_unsubcribe(&g_protocol_uart_rf.RAW_SEND, rf_update_send_callback);
            }
        }
    }
}

// 透传初始化
static void transparent_init(void)
{
    os_task_create(transparent_entry, "rf_head_transparent", NULL, OS_PRIORITY_BSP, OS_TASK_STACK_MIN);
}

/**
 * @file protocol_test.cc
 * @author WittXie
 * @brief 协议测试
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"
protocol_t *s_protocol_group[] = {&g_protocol_uart_rf, &g_protocol_uart_head, &g_protocol_uart_sport};

static void protocol_entry(void *args)
{
    for (;;)
    {
        os_sleep(3000);
        if (g_adc_sw_left < 0.75)
            continue;

        if (gpio_read(&g_btn_slw_left) == false)
            continue;

        print("\r\n\n\n");
        for (int i = 0; i < countof(s_protocol_group); i++)
        {
            os_sleep(10);
            protocol_frame_t frame = {
                .cmd = 0xAA,
                .data = (uint8_t *)s_protocol_group[i]->cfg.name,
                .data_length = strlen(s_protocol_group[i]->cfg.name),
            };
            protocol_send(s_protocol_group[i], &frame);
            print("\r[%s]send: cmd[0x%02X], data_length[%d], data[%s]." ASCII_CLEAR_TAIL "\r\n",
                  s_protocol_group[i]->cfg.name, frame.cmd, frame.data_length, frame.data);
        }
        print("\r\n");
    }
}
static void protocol_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    protocol_t *protocol = (protocol_t *)device;
    protocol_frame_t *frame = (protocol_frame_t *)arg;
    frame->data[frame->data_length] = '\0';
    print("\r[%s]recv: cmd[0x%02X], data_length[%d], data[%s]." ASCII_CLEAR_TAIL "\r\n",
          protocol->cfg.name, frame->cmd, frame->data_length, frame->data);
}
static void protocol_test(void)
{
    os_task_create(protocol_entry, "protocol_test", NULL, OS_PRIORITY_APP, OS_TASK_STACK_MIN + 2048);

    // 注册订阅
    for (int i = 0; i < countof(s_protocol_group); i++)
    {
        dds_subcribe(&s_protocol_group[i]->RECEIVE, DDS_PRIORITY_NORMAL, &protocol_callback, NULL);
    }
}

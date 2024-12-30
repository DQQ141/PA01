/**
 * @file 富斯RF HEAD协议驱动.cc
 * @author WittXie
 * @brief 协议串口驱动
 * @version 0.1
 * @date 2024-10-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../bsp.h"
#include "./../protocol_bsp.h"

#define UART_HEAD huart6
#define UART_HEAD_DMA_BUFF_SIZE 1024
static uint8_t uart_head_write_buff[UART_HEAD_DMA_BUFF_SIZE] __section_sdram = {0};
static uint8_t uart_head_read_buff[UART_HEAD_DMA_BUFF_SIZE] __section_sdram = {0};

// HEAD CMD  DATA_LENGTH DATA                                                   CRC
// 55   C4   09          7D   FD   8C   F5     23   FF   9B   00      11        4D   3B
static uint32_t uart_head_pack(protocol_frame_t *frame, uint8_t *send_buff)
{
    uint32_t send_length = frame->data_length + g_protocol_uart_head.cfg.frame_min;

    // 填装数据
    send_buff[0] = g_protocol_uart_head.cfg.head_code;
    send_buff[1] = frame->cmd;
    send_buff[2] = frame->data_length;
    memcpy(&send_buff[3], frame->data, frame->data_length);

    // CRC 校验
    uint32_t ret_crc = crc_calculate(&g_crc_ccitt, send_buff, send_length - 2);
    send_buff[send_length - 1] = (ret_crc >> 8) & 0xFF;
    send_buff[send_length - 2] = ret_crc & 0xFF;
    return send_length;
}

static uint32_t uart_head_unpack(protocol_frame_t *frame, uint8_t *recv_buff, uint32_t recv_length)
{
    // 第三个字节是长度
    if (recv_length < recv_buff[2] + g_protocol_uart_head.cfg.frame_min) // 长度 >= 数据长度 + 帧头长度 + 指令长度 + CRC校验长度
    {
        return 0;
    }

    recv_length = recv_buff[2] + g_protocol_uart_head.cfg.frame_min;

    // CRC 校验
    uint32_t ret_crc = crc_calculate(&g_crc_ccitt, recv_buff, recv_length - 2);
    if (ret_crc != (recv_buff[recv_length - 1] << 8 | recv_buff[recv_length - 2]))
    {
        return 10;
    }

    // 解包
    frame->cmd = recv_buff[1];
    frame->data_length = recv_buff[2];
    memcpy(frame->data, &recv_buff[3], frame->data_length);

    // 校验成功
    return recv_length;
}

static void uart_head_dma_receive(void)
{
    static uint32_t last_cnt = 0; // 保存上一次计数器值
    uint32_t current_cnt;
    uint32_t received_length;

    if (protocol_is_inited(&g_protocol_uart_head) == true && protocol_is_running(&g_protocol_uart_head) == false)
    {
        // 清空DMA
        HAL_UART_AbortReceive(&(UART_HEAD));
        // SCB_InvalidateDCache_by_Addr((uint32_t *)uart_head_read_buff, UART_HEAD_DMA_BUFF_SIZE); //关闭部分内存的cache
        HAL_UART_Receive_DMA(&(UART_HEAD), uart_head_read_buff, UART_HEAD_DMA_BUFF_SIZE);
        last_cnt = 0;
        return;
    }

    // 获取当前DMA传输的计数器值
    current_cnt = UART_HEAD_DMA_BUFF_SIZE - __HAL_DMA_GET_COUNTER(UART_HEAD.hdmarx); // huart6句柄

    if (current_cnt == last_cnt)
        return;

    // 如果当前接收到的数据长度大于上一次，说明有新的数据接收到
    if (current_cnt > last_cnt)
    {
        received_length = current_cnt - last_cnt; // 计算接收到的数据长度

        // 调用协议处理函数，将接收的数据传递出去
        protocol_read_hook(&g_protocol_uart_head, &uart_head_read_buff[last_cnt], received_length);
    }
    else
    {
        // 如果计数器回绕（循环接收），需分两段处理：
        // 第一段是从 last_cnt 到缓冲区末尾的数据
        received_length = UART_HEAD_DMA_BUFF_SIZE - last_cnt;
        protocol_read_hook(&g_protocol_uart_head, &uart_head_read_buff[last_cnt], received_length);

        // 第二段是从缓冲区起始位置到 current_cnt 的数据
        received_length = current_cnt;
        protocol_read_hook(&g_protocol_uart_head, &uart_head_read_buff[0], received_length);

        received_length += UART_HEAD_DMA_BUFF_SIZE - last_cnt;
    }

    // 更新上一次计数器值
    last_cnt = current_cnt;
}

// 接收改成DMA循环模式
static void uart_head_init()
{
    dds_subcribe(&g_protocol_uart_head.POLL, DDS_PRIORITY_NORMAL, (dds_callback_t)uart_head_dma_receive, NULL);
}

// 发送数据
static void uart_head_write(uint8_t *buff, uint32_t length)
{
    os_sleep_until(__HAL_UART_GET_FLAG(&(UART_HEAD), UART_FLAG_TXE), 1000);
    os_sleep_until(__HAL_UART_GET_FLAG(&(UART_HEAD), UART_FLAG_TC), 1000);
    (UART_HEAD).gState = HAL_UART_STATE_READY;
    memcpy(uart_head_write_buff, buff, length);
    HAL_UART_Transmit(&(UART_HEAD), uart_head_write_buff, length, 1000);
}
protocol_t g_protocol_uart_head = {
    .cfg = {
        .name = "g_protocol_uart_head",
        .buff_size = UART_HEAD_DMA_BUFF_SIZE,
        .frame_min = 5,
        .frame_max = 256,
        .head_code = 0x55,
    },
    .ops = {
        .init = uart_head_init,
        .write = uart_head_write,
        .pack = uart_head_pack,
        .unpack = uart_head_unpack,
    },
};

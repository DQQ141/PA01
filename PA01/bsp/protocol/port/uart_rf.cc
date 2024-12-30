/**
 * @file 富斯RF协议驱动.cc
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

#define UART_RF huart4
#define UART_RF_DMA_BUFF_SIZE 1024
static uint8_t uart_rf_write_buff[UART_RF_DMA_BUFF_SIZE] __section_sdram = {0};
static uint8_t uart_rf_read_buff[UART_RF_DMA_BUFF_SIZE] __section_sdram = {0};
static uint8_t uart_rf_unpack_buff[UART_RF_DMA_BUFF_SIZE] = {0};

// CHECKSUM = Address到DATAn全部加起来的和再取反
#define FS_INRM303_CHECKSUM(_data, _length) ({ \
    uint8_t _sum = 0;                          \
    for (uint32_t i = 0; i < _length; i++)     \
    {                                          \
        _sum += (_data)[i];                    \
    }                                          \
    _sum ^ 0xFF;                               \
})

// 转义
static inline uint32_t rf_escape_exchange(uint8_t *buff, uint32_t length)
{
    uint32_t esc_cnt = 0;
    for (uint32_t i = 1; i < length - 1; i++)
    {
        if (buff[i] == FS_INRM303_END) // ESC_END 0xDC 发送数据中有END相同的字符，则先发ESC字符，发完后再发ESC_END字符
        {
            buff[i] = FS_INRM303_ESC;
            memmove(buff + i + 1, buff + i, length - i);
            buff[i + 1] = FS_INRM303_ESC_END;
            esc_cnt++;
        }
        else if (buff[i] == FS_INRM303_ESC) // ESC_ESC 0xDD 发送数据中有ESC相同的字符，则先发ESC字符，发完后再发ESC_ESC字符
        {
            buff[i] = FS_INRM303_ESC;
            memmove(buff + i + 1, buff + i, length - i);
            buff[i + 1] = FS_INRM303_ESC_ESC;
            esc_cnt++;
        }
    }
    length += esc_cnt;

    return length;
}

// 反转义
static inline uint32_t rf_escape_unexchange(uint8_t *buff, uint32_t length)
{
    uint32_t esc_cnt = 0;
    for (uint32_t i = 1; i < length - 1; i++)
    {
        if (buff[i] == FS_INRM303_ESC)
        {
            if (buff[i + 1] == FS_INRM303_ESC_END)
            {
                buff[i] = FS_INRM303_END;
                memmove(buff + i + 1, buff + i + 2, length - i - 1);
                esc_cnt++;
            }
            else if (buff[i + 1] == FS_INRM303_ESC_ESC)
            {
                buff[i] = FS_INRM303_ESC;
                memmove(buff + i + 1, buff + i + 2, length - i - 1);
                esc_cnt++;
            }
        }
    }
    length -= esc_cnt;

    return length;
}

// 帧格式
// END | Address | Frame Number | Frame Type | Protocol ID | DATA0~DATAn | CHECKSUM | END
static uint32_t uart_rf_pack(protocol_frame_t *frame, uint8_t *send_buff)
{
    uint32_t send_length = 0;
    uint32_t check_sum = 0;

    frame->number = g_fs_inrm303.frame_cnt++;
    send_buff[send_length++] = FS_INRM303_END;
    send_buff[send_length++] = (frame->addr_dst << 4 | frame->addr_src);
    send_buff[send_length++] = frame->number;                         // 帧号
    send_buff[send_length++] = (frame->cmd >> 8) & 0xFF;              // Frame Type
    send_buff[send_length++] = frame->cmd & 0xFF;                     // Protocol ID
    memcpy(send_buff + send_length, frame->data, frame->data_length); // DATA0~DATAn
    send_length += frame->data_length;                                // 数据长度

    // CHECKSUM = Address到DATAn全部加起来的和再取反
    send_buff[send_length++] = FS_INRM303_CHECKSUM(send_buff + 1, send_length - 1);
    send_buff[send_length++] = FS_INRM303_END;

    send_length = rf_escape_exchange(send_buff, send_length);
    return send_length;
}

static uint32_t uart_rf_unpack(protocol_frame_t *frame, uint8_t *recv_buff, uint32_t recv_length)
{
    if (recv_length < g_protocol_uart_rf.cfg.frame_min)
    {
        return 0;
    }

    memcpy(uart_rf_unpack_buff, recv_buff, recv_length);

    // 帧最小长度校验
    if (recv_length < g_protocol_uart_rf.cfg.frame_min)
    {
        return 0;
    }

    // 帧头校验
    if (uart_rf_unpack_buff[0] != FS_INRM303_END)
    {
        return 0;
    }

    // 查找帧尾
    uint32_t end_index = 0;
    for (uint32_t i = g_protocol_uart_rf.cfg.frame_min - 1; i < recv_length; i++)
    {
        if (uart_rf_unpack_buff[i] == FS_INRM303_END && uart_rf_unpack_buff[i - 1] != FS_INRM303_ESC)
        {
            end_index = i + 1;
            break;
        }
    }
    if (end_index == 0)
    {
        return 0;
    }

    // 数据转换成原始数据
    recv_length = rf_escape_unexchange(uart_rf_unpack_buff, end_index);

    // CHECKSUM校验, CHECKSUM = Address 到 DATAn
    // END [ Address | Frame Number | Frame Type | Protocol ID | DATA0~DATAn ] CHECKSUM | END
    if (FS_INRM303_CHECKSUM(uart_rf_unpack_buff + 1, recv_length - 3) != uart_rf_unpack_buff[recv_length - 2])
    {
        return 0;
    }

    // 检查地址
    if ((uart_rf_unpack_buff[1] >> 4) != FS_INRM303_ADDR_MAIN)
    {
        return 0;
    }

    // 解包
    // END | Address | Frame Number | Frame Type | Protocol ID | DATA0~DATAn | CHECKSUM | END
    frame->addr_dst = uart_rf_unpack_buff[1] >> 4;                                   // Address
    frame->addr_src = uart_rf_unpack_buff[1] & 0x0F;                                 // Address
    frame->number = uart_rf_unpack_buff[2];                                          // Frame Number
    frame->cmd = FS_INRM303_CMD_GET(uart_rf_unpack_buff[3], uart_rf_unpack_buff[4]); // Frame Type | Protocol ID
    frame->data_length = recv_length - g_protocol_uart_rf.cfg.frame_min;             // 数据长度
    memcpy(frame->data, uart_rf_unpack_buff + 5, frame->data_length);                // 转义后，存储到 frame->data[] 中

    return end_index;
}

static void uart_rf_dma_receive(void)
{

    static uint32_t last_cnt = 0; // 保存上一次计数器值
    uint32_t current_cnt;
    uint32_t received_length;

    if (protocol_is_inited(&g_protocol_uart_rf) == true && protocol_is_running(&g_protocol_uart_rf) == false)
    {
        HAL_UART_AbortReceive(&(UART_RF));
        HAL_UART_Receive_DMA(&(UART_RF), uart_rf_read_buff, UART_RF_DMA_BUFF_SIZE);
        last_cnt = 0;
        return;
    }

    // 获取当前DMA传输的计数器值
    current_cnt = UART_RF_DMA_BUFF_SIZE - __HAL_DMA_GET_COUNTER(UART_RF.hdmarx); // huart4句柄

    if (current_cnt == last_cnt)
        return;

    // 如果当前接收到的数据长度大于上一次，说明有新的数据接收到
    if (current_cnt > last_cnt)
    {
        received_length = current_cnt - last_cnt; // 计算接收到的数据长度

        // 调用协议处理函数，将接收的数据传递出去
        protocol_read_hook(&g_protocol_uart_rf, &uart_rf_read_buff[last_cnt], received_length);
    }
    else
    {
        // 如果计数器回绕（循环接收），需分两段处理：
        // 第一段是从 last_cnt 到缓冲区末尾的数据
        received_length = UART_RF_DMA_BUFF_SIZE - last_cnt;
        protocol_read_hook(&g_protocol_uart_rf, &uart_rf_read_buff[last_cnt], received_length);

        // 第二段是从缓冲区起始位置到 current_cnt 的数据
        received_length = current_cnt;
        protocol_read_hook(&g_protocol_uart_rf, &uart_rf_read_buff[0], received_length);
    }

    // 更新上一次计数器值
    last_cnt = current_cnt;
}

// 接收改成DMA循环模式
static void uart_rf_init()
{
    dds_subcribe(&g_protocol_uart_rf.POLL, DDS_PRIORITY_NORMAL, (dds_callback_t)uart_rf_dma_receive, NULL);
    fs_inrm303_init(&g_fs_inrm303);
}
static void uart_rf_write(uint8_t *buff, uint32_t length)
{
    os_sleep_until(__HAL_UART_GET_FLAG(&(UART_RF), UART_FLAG_TXE), 1000);
    os_sleep_until(__HAL_UART_GET_FLAG(&(UART_RF), UART_FLAG_TC), 1000);
    (UART_RF).gState = HAL_UART_STATE_READY;
    memcpy(uart_rf_write_buff, buff, length);
    HAL_UART_Transmit_DMA(&(UART_RF), uart_rf_write_buff, length);
}
protocol_t g_protocol_uart_rf = {
    .cfg = {
        .name = "g_protocol_uart_rf",
        .buff_size = UART_RF_DMA_BUFF_SIZE,
        .frame_min = 7,
        .frame_max = 512,
        .head_code = FS_INRM303_END,
    },
    .ops = {
        .init = uart_rf_init,
        .write = uart_rf_write,
        .pack = uart_rf_pack,
        .unpack = uart_rf_unpack,
    },
};

static void inrm303_poll_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_poll(&g_fs_inrm303);
}

static void inrm303_receive_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    protocol_frame_t *frame = (protocol_frame_t *)arg;
    fs_inrm303_read_hook(&g_fs_inrm303, frame->cmd, frame->data, frame->data_length);
}

static void inrm303_init(void)
{
    // 连接到协议
    dds_subcribe(&g_protocol_uart_rf.POLL, DDS_PRIORITY_NORMAL, inrm303_poll_callback, NULL);
    dds_subcribe(&g_protocol_uart_rf.RECEIVE, DDS_PRIORITY_NORMAL, inrm303_receive_callback, NULL);
}
static void inrm303_write(uint32_t cmd, uint8_t *data, uint32_t data_length)
{
    protocol_frame_t frame = {0};
    frame.data = data;
    frame.cmd = cmd;
    frame.addr_dst = FS_INRM303_ADDR_RF;
    frame.addr_src = FS_INRM303_ADDR_MAIN;
    frame.data_length = data_length;
    protocol_send(&g_protocol_uart_rf, &frame);
}
fs_inrm303_t g_fs_inrm303 = {
    .cfg = {
        .name = "g_fs_inrm303",
        .try_cnt = 10,
        .channel_size = 18,
    },
    .ops = {
        .init = inrm303_init,
        .write = inrm303_write,
    },
};

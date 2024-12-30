/**
 * @file rf_info.cc
 * @author WittXie
 * @brief RF信息获取
 * @version 0.1
 * @date 2024-11-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../factory_app.h"

static void info_receive_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    protocol_t *protocol = (protocol_t *)device;
    protocol_frame_t *frame = (protocol_frame_t *)arg;

    // print_hex8("%s", frame->data, frame->data_length, protocol->cfg.name);
}

// 信息初始化
static void rf_info_init(void)
{
    // 订阅串口接收
    dds_subcribe(&g_protocol_uart_rf.RECEIVE, DDS_PRIORITY_NORMAL, info_receive_callback, NULL);
}

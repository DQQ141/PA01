/**
 * @file rf_debug.cc
 * @author WittXie
 * @brief RF调试
 * @version 0.1
 * @date 2024-11-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../factory_app.h"

static bool is_rf_debug_ignore(protocol_frame_t *frame)
{
    if (((frame->cmd & 0xFF) == FS_INRM303_CID_READY) ||
        (frame->cmd == FS_INRM303_CMD_GET(FS_INRM303_TYPE_READ, FS_INRM303_CID_STATUS)) ||
        (frame->cmd == FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_PARAM, FS_INRM303_CID_STATUS)) ||
        (frame->cmd == FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_NONE, FS_INRM303_CID_STATUS)) ||
        (frame->cmd == FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_NONE, FS_INRM303_CID_STATUS)) ||
        ((frame->cmd & 0xFF) == FS_INRM303_CID_VERSION_INFO) ||
        ((frame->cmd & 0xFF) == 0x0B) ||
        ((frame->cmd & 0xFF) == FS_INRM303_CID_RX_DATA) ||
        ((frame->cmd & 0xFF) == FS_INRM303_CID_TX_DATA))
    {
        return true;
    }
    return false;
}

static void rf_debug_send_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    protocol_t *protocol = (protocol_t *)device;
    protocol_frame_t *frame = (protocol_frame_t *)arg;

    if (is_rf_debug_ignore(frame))
    {
        return;
    }

    print_hex8(COLOR_H_MAGENTA "%s> send[%d], fsn[%d], cmd[%04X]", frame->data, frame->data_length,
               protocol->cfg.name, frame->data_length, frame->number, frame->cmd);
}

static void rf_debug_raw_send_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    protocol_t *protocol = (protocol_t *)device;
    protocol_frame_t *frame = (protocol_frame_t *)arg;

    print_hex8(COLOR_H_WHITE "%s> raw send[%d]", frame->data, frame->data_length,
               protocol->cfg.name, frame->data_length);
}

static void rf_debug_receive_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    protocol_t *protocol = (protocol_t *)device;
    protocol_frame_t *frame = (protocol_frame_t *)arg;

    if (is_rf_debug_ignore(frame))
    {
        return;
    }

    print_hex8(COLOR_H_GREEN "%s> received[%d], fsn[%d], cmd[%04X]", frame->data, frame->data_length,
               protocol->cfg.name, frame->data_length, frame->number, frame->cmd);
}

static void rf_debug_raw_receive_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    protocol_t *protocol = (protocol_t *)device;
    protocol_frame_t *frame = (protocol_frame_t *)arg;

    print_hex8(COLOR_H_WHITE "%s> raw received[%d]", frame->data, frame->data_length,
               protocol->cfg.name, frame->data_length);
}

// 调试初始化
static void rf_debug_init(void)
{
    // dds_subcribe(&g_protocol_uart_rf.SEND, DDS_PRIORITY_NORMAL, rf_debug_send_callback, NULL);
    // dds_subcribe(&g_protocol_uart_rf.RAW_SEND, DDS_PRIORITY_NORMAL, rf_debug_raw_send_callback, NULL);
    // dds_subcribe(&g_protocol_uart_rf.RECEIVE, DDS_PRIORITY_NORMAL, rf_debug_receive_callback, NULL);
    // dds_subcribe(&g_protocol_uart_rf.RAW_RECEIVE, DDS_PRIORITY_NORMAL, rf_debug_raw_receive_callback, NULL);
}

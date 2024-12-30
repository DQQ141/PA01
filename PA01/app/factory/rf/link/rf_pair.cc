/**
 * @file rf_pair.cc
 * @author WittXie
 * @brief 对码
 * @version 0.1
 * @date 2024-12-04
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../factory_app.h"

static void rf_pair_receive_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    protocol_t *protocol = (protocol_t *)device;
    protocol_frame_t *frame = (protocol_frame_t *)arg;

    if ((frame->cmd != FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_PARAM, FS_INRM303_CID_STATUS)) &&
        (frame->cmd != FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_NONE, FS_INRM303_CID_STATUS)))
    {
        return;
    }

    if (CMP_PREV(g_fs_inrm303.data.status.status) == 0)
    {
        return;
    }

    switch (g_fs_inrm303.data.status.status)
    {
    case FS_INRM303_STATUS_HARDWARE_ERROR: // 硬件错误
        log_info(COLOR_H_RED "inrm303 hardware error.");
        break;

    case FS_INRM303_STATUS_PAIRING: // 对码中
        log_info(COLOR_H_CYAN "inrm303 pairing.");
        break;
    case FS_INRM303_STATUS_SYNC: // 同步中
        log_info(COLOR_H_CYAN "inrm303 syncing.");
        break;

    case FS_INRM303_STATUS_SYNCED: // 已同步
        log_info(COLOR_H_CYAN "inrm303 synced.");
        break;

    case FS_INRM303_STATUS_STANDBY: // 待机
        log_info(COLOR_H_CYAN "inrm303 standby.");
        break;

    case FS_INRM303_STATUS_RF_HEAD_WAITING_UPDATE: // 等待RF头更新
        log_info(COLOR_H_CYAN "inrm303 waiting for RF head update.");
        break;

    case FS_INRM303_STATUS_RF_HEAD_UPDATING: // RF头更新中
        log_info(COLOR_H_CYAN "inrm303 RF head updating.");
        break;

    case FS_INRM303_STATUS_RF_HEAD_UPDATE_RECEIVER: // RF头更新接收机
        log_info(COLOR_H_CYAN "inrm303 RF head update receiver.");
        break;

    case FS_INRM303_STATUS_RF_HEAD_UPDATE_RECEIVER_FAILED: // RF头更新接收机失败
        log_info(COLOR_H_CYAN "inrm303 RF head update receiver failed.");
        break;

    case FS_INRM303_STATUS_RF_HEAD_RF_TEST: // RF测试
        log_info(COLOR_H_CYAN "inrm303 RF test.");
        break;

    case FS_INRM303_STATUS_HARDWARE_TEST: // 硬件测试
        log_info(COLOR_H_CYAN "inrm303 hardware test.");
        break;

    default:
        log_info(COLOR_H_CYAN "unknown status: 0x%02X", g_fs_inrm303.data.status.status);
        break;
    }
}
static void rf_pair_btn_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_pair_v1(&g_fs_inrm303);
}
// 对码初始化
static void rf_pair_init(void)
{
    // dds_subcribe(&g_btn_power.PRESSED, DDS_PRIORITY_NORMAL, rf_pair_btn_callback, NULL);

    // 订阅串口接收
    dds_subcribe(&g_protocol_uart_rf.RECEIVE, DDS_PRIORITY_NORMAL, rf_pair_receive_callback, NULL);
}

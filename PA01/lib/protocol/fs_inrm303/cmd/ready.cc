/**
 * @file ready.c
 * @author WittXie
 * @brief 准备就绪
 * @version 0.1
 * @date 2024-12-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../fs_inrm303.h"

// 监听指令
static void fs_inrm303_ready_received_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;
    fs_inrm303_received_t *received = (fs_inrm303_received_t *)arg;

    uint32_t cmd = received->cmd;
    uint8_t *data = received->data;
    uint32_t data_length = received->data_length;

    if ((cmd & 0xFF) != FS_INRM303_CID_READY)
    {
        return;
    }

    if (data_length != FS_INRM303_DATA_LENGTH_READY)
    {
        INFO("cmd: 0x%04X data_length: 0x%02X != 0x%02X", cmd, data_length, FS_INRM303_DATA_LENGTH_READY);
        return;
    }
    FS_INRM303_DATA_SET_READY(&fs_inrm303->data.ready, data);
    fs_inrm303->timestamp.rf_test = 0; // 清除时间戳
}

static void fs_inrm303_init_cmd_ready(fs_inrm303_t *fs_inrm303)
{
    dds_subcribe(&fs_inrm303->RECEIVED, DDS_PRIORITY_NORMAL, fs_inrm303_ready_received_cb, NULL);
}

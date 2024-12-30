/**
 * @file fs_inrm303_info.c
 * @brief 处理版本信息指令
 */

#include "./../fs_inrm303.h"

// 监听版本信息指令
static void fs_inrm303_info_received_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;
    fs_inrm303_received_t *received = (fs_inrm303_received_t *)arg;

    uint32_t cmd = received->cmd;
    uint8_t *data = received->data;
    uint32_t data_length = received->data_length;

    if (cmd != FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_PARAM, FS_INRM303_CID_VERSION_INFO))
    {
        return;
    }

    if (data_length == FS_INRM303_DATA_LENGTH_INFO)
    {
        FS_INRM303_DATA_SET_INFO(&fs_inrm303->data.info, data);
    }
    else
    {
        // 打印错误信息
        INFO("cmd: 0x%04X data_length: 0x%02X != 0x%02X", cmd, data_length, FS_INRM303_DATA_LENGTH_INFO);
    }
}

static void fs_inrm303_init_cmd_info(fs_inrm303_t *fs_inrm303)
{
    dds_subcribe(&fs_inrm303->RECEIVED, DDS_PRIORITY_NORMAL, fs_inrm303_info_received_cb, NULL);
}

/**
 * @file status.c
 * @brief 处理询问主机状态的指令
 */

#include "./../fs_inrm303.h"

// 监听询问主机状态指令
static void fs_inrm303_status_received_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;
    fs_inrm303_received_t *received = (fs_inrm303_received_t *)arg;

    uint32_t cmd = received->cmd;
    uint8_t *data = received->data;
    uint32_t data_length = received->data_length;

    if (cmd == FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_NONE, FS_INRM303_CID_STATUS))
    {
        if (data_length == FS_INRM303_DATA_LENGTH_STATUS)
        {
            FS_INRM303_DATA_SET_STATUS(&fs_inrm303->data.status, data);
            // fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_NONE, FS_INRM303_CID_STATUS), NULL, 0); // 回复RF状态
        }
        else
        {
            // 打印错误信息
            INFO("cmd: 0x%04X data_length: 0x%02X != 0x%02X", cmd, data_length, FS_INRM303_DATA_LENGTH_STATUS);
        }
    }
    else if (cmd == FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_PARAM, FS_INRM303_CID_STATUS))
    {
        if (data_length == FS_INRM303_DATA_LENGTH_STATUS)
        {
            FS_INRM303_DATA_SET_STATUS(&fs_inrm303->data.status, data);
        }
        else
        {
            // 打印错误信息
            INFO("cmd: 0x%04X data_length: 0x%02X != 0x%02X", cmd, data_length, FS_INRM303_DATA_LENGTH_STATUS);
        }
    }
}

static void fs_inrm303_status_poll_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;
    if (is_timeout(fs_inrm303->timestamp.status, 100000))
    {
        fs_inrm303_update(fs_inrm303); // 更新数据
        fs_inrm303->timestamp.status = TIMESTAMP_US_GET();
    }
}

static void fs_inrm303_init_cmd_status(fs_inrm303_t *fs_inrm303)
{
    dds_subcribe(&fs_inrm303->RECEIVED, DDS_PRIORITY_NORMAL, fs_inrm303_status_received_cb, NULL);
    dds_subcribe(&fs_inrm303->POLL, DDS_PRIORITY_NORMAL, fs_inrm303_status_poll_cb, NULL);
}

/**
 * @file fs_inrm303_mode.c
 * @brief 处理模式设置指令
 */

#include "./../fs_inrm303.h"

void fs_inrm303_send_cmd_mode(fs_inrm303_t *fs_inrm303, fs_inrm303_mode_enum_t mode)
{
    ASSERT(fs_inrm303 != NULL);
    ASSERT(mode != 0);
    fs_inrm303->data.mode.mode = mode;
    fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_MODE),
                          (uint8_t[]){fs_inrm303->data.mode.mode}, FS_INRM303_DATA_LENGTH_MODE);

    fs_inrm303->timestamp.mode = TIMESTAMP_US_GET(); // 开始计时，超时重发
}

static void fs_inrm303_mode_poll_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;

    if (fs_inrm303->timestamp.mode == 0)
    {
        return;
    }

    if (is_timeout(fs_inrm303->timestamp.mode, 500000))
    {
        fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_MODE),
                              (uint8_t[]){fs_inrm303->data.mode.mode}, FS_INRM303_DATA_LENGTH_MODE);
        fs_inrm303->timestamp.mode = TIMESTAMP_US_GET();
    }
}

// 监听模式设置指令
static void fs_inrm303_mode_received_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;
    fs_inrm303_received_t *received = (fs_inrm303_received_t *)arg;

    uint32_t cmd = received->cmd;
    uint8_t *data = received->data;
    uint32_t data_length = received->data_length;

    if (cmd != FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_PARAM, FS_INRM303_CID_MODE))
    {
        return;
    }

    if (data_length == FS_INRM303_DATA_LENGTH_MODE)
    {
        if (data[0] == 0x02) // 成功
        {
            fs_inrm303->timestamp.mode = 0; // 清除时间戳
        }
    }
    else
    {
        // 打印错误信息
        INFO("cmd: 0x%04X data_length: 0x%02X != 0x%02X", cmd, data_length, FS_INRM303_DATA_LENGTH_MODE);
    }
}

static void fs_inrm303_init_cmd_mode(fs_inrm303_t *fs_inrm303)
{
    dds_subcribe(&fs_inrm303->POLL, DDS_PRIORITY_NORMAL, fs_inrm303_mode_poll_cb, NULL);
    dds_subcribe(&fs_inrm303->RECEIVED, DDS_PRIORITY_NORMAL, fs_inrm303_mode_received_cb, NULL);
}

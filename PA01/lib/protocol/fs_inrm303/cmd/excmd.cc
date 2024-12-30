/**
 * @file excmd.c
 * @author WittXie
 * @brief 拓展指令
 * @version 0.1
 * @date 2024-12-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../fs_inrm303.h"

// 进入测试模式
void fs_inrm303_send_cmd_power(fs_inrm303_t *fs_inrm303, uint16_t power)
{
    fs_inrm303->data.excmd.power = power;
    fs_inrm303->data.excmd.code = SET_RF_OUTPUT_POWER;
    fs_inrm303->data.excmd.code_length = CODE_LENGTH_RF_OUTPUT_POWER;
    fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_COMMAND), // 需要参数应答
                          (uint8_t *)&fs_inrm303->data.excmd,
                          FS_INRM303_DATA_LENGTH_EXCMD(CODE_LENGTH_RF_OUTPUT_POWER));

    fs_inrm303->timestamp.excmd = TIMESTAMP_US_GET(); // 记录时间戳
}

// 轮询指令
static void fs_inrm303_excmd_poll_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;

    if (fs_inrm303->timestamp.excmd == 0)
    {
        // 不需要应答或者已经应答
        return;
    }

    if (is_timeout(fs_inrm303->timestamp.excmd, 500000))
    {
        fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_COMMAND), // 需要参数应答
                              (uint8_t *)&fs_inrm303->data.excmd,
                              FS_INRM303_DATA_LENGTH_EXCMD(CODE_LENGTH_RF_OUTPUT_POWER));
        fs_inrm303->timestamp.excmd = TIMESTAMP_US_GET();
    }
}

// 监听指令
static void fs_inrm303_excmd_received_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;
    fs_inrm303_received_t *received = (fs_inrm303_received_t *)arg;

    uint32_t cmd = received->cmd;
    uint8_t *data = received->data;
    uint32_t data_length = received->data_length;

    if (cmd != FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_PARAM, FS_INRM303_CID_COMMAND))
    {
        return;
    }
    fs_inrm303->timestamp.excmd = 0; // 清除时间戳
}

static void fs_inrm303_init_excmd(fs_inrm303_t *fs_inrm303)
{
    memset(&fs_inrm303->data.excmd, 0, sizeof(fs_inrm303->data.excmd));

    dds_subcribe(&fs_inrm303->POLL, DDS_PRIORITY_NORMAL, fs_inrm303_excmd_poll_cb, NULL);
    dds_subcribe(&fs_inrm303->RECEIVED, DDS_PRIORITY_NORMAL, fs_inrm303_excmd_received_cb, NULL);
}

/**
 * @file pair.c
 * @author WittXie
 * @brief 对码
 * @version 0.1
 * @date 2024-12-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../fs_inrm303.h"

void fs_inrm303_pair_v1(fs_inrm303_t *fs_inrm303)
{
    // 先发送对码参数配置
    fs_inrm303->data.pair_config.Version = 1;
    fs_inrm303->data.pair_config.ConfigV1.EMIStandard = LNK_ES_CE;
    fs_inrm303->data.pair_config.ConfigV1.IsTwoWay = true;
    fs_inrm303->data.pair_config.ConfigV1.PhyMode = ROUTINE_FLCR1_18CH;
    fs_inrm303->data.pair_config.ConfigV1.SignalStrengthRCChannelNb = 13;
    fs_inrm303->data.pair_config.ConfigV1.FailsafeTimeout = 500;
    for (int i = 0; i < MAX_RF_CHANNELS_NUMBER; i++) // 设置默认的 failsafe 值
    {
        fs_inrm303->data.pair_config.ConfigV1.FailSafe[i] = 0x8000; // For 6-bit channels and more only, outputs low or high level depending on the interface
    }
    fs_inrm303->data.pair_config.ConfigV1.FailsafeOutputMode = true;
    for (int i = 0; i < SES_NPT_NB_MAX_PORTS; i++) // 设置默认的 PWM 频率
    {
        fs_inrm303->data.pair_config.ConfigV1.NewPortTypes[i] = SES_NPT_PWM; // 设置默认的端口类型
    }
    for (int i = 0; i < SES_NB_MAX_CHANNELS; i++) // 设置默认的 PWM 频率
    {
        fs_inrm303->data.pair_config.ConfigV1.PWMFrequenciesV1.PWMFrequencies[i] = 50;
    }
    fs_inrm303->data.pair_config.ConfigV1.PWMFrequenciesV1.Synchronized = 0;
    fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_PAIR_PARAM_CONFIG),
                          (uint8_t *)&fs_inrm303->data.pair_config, sizeof(fs_inrm303->data.pair_config.ConfigV1));
    // 开始对码
    fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_MODE),
                          (uint8_t[]){FS_INRM303_MODE_PAIRING}, FS_INRM303_DATA_LENGTH_MODE);

    fs_inrm303->timestamp.pair = TIMESTAMP_US_GET(); // 记录时间戳
}

void fs_inrm303_pair_v0(fs_inrm303_t *fs_inrm303)
{
    // 先发送对码参数配置
    fs_inrm303->data.pair_config.Version = 0;
    fs_inrm303->data.pair_config.ConfigV0.EMIStandard = LNK_ES_CE;
    fs_inrm303->data.pair_config.ConfigV0.IsTwoWay = true;
    fs_inrm303->data.pair_config.ConfigV0.PhyMode = ROUTINE_FLCR1_18CH;
    fs_inrm303->data.pair_config.ConfigV0.SignalStrengthRCChannelNb = 13;
    fs_inrm303->data.pair_config.ConfigV0.FailsafeTimeout = 500;
    for (int i = 0; i < MAX_RF_CHANNELS_NUMBER; i++) // 设置默认的 failsafe 值
    {
        fs_inrm303->data.pair_config.ConfigV0.FailSafe[i] = 0x8000; // For 6-bit channels and more only, outputs low or high level depending on the interface
    }
    fs_inrm303->data.pair_config.ConfigV0.FailsafeOutputMode = true;
    fs_inrm303->data.pair_config.ConfigV0.PWMFrequency.Frequency = 50;
    fs_inrm303->data.pair_config.ConfigV0.PWMFrequency.Synchronized = 0;
    fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_PAIR_PARAM_CONFIG),
                          (uint8_t *)&fs_inrm303->data.pair_config, sizeof(fs_inrm303->data.pair_config.ConfigV0));

    // 开始对码
    fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_MODE),
                          (uint8_t[]){FS_INRM303_MODE_PAIRING}, FS_INRM303_DATA_LENGTH_MODE);

    fs_inrm303->timestamp.pair = TIMESTAMP_US_GET(); // 记录时间戳
}

void fs_inrm303_chnnel_set(fs_inrm303_t *fs_inrm303, uint8_t channel, int16_t value)
{
    ASSERT(channel <= fs_inrm303->cfg.channel_size && channel > 0, "channel out of range");

    if (value < -15000)
        value = -15000;
    if (value > 15000)
        value = 15000;

    fs_inrm303->data.tx_data.channel[channel - 1] = value;
}

void fs_inrm303_chnnel_us_set(fs_inrm303_t *fs_inrm303, uint8_t channel, int16_t us)
{
    ASSERT(channel <= fs_inrm303->cfg.channel_size && channel > 0, "channel out of range");

    // 750us~2250us <=> -15000~15000
    if (us < 750)
        us = 750;
    if (us > 2250)
        us = 2250;

    fs_inrm303->data.tx_data.channel[channel - 1] = (int16_t)((us - 1500) * 20);
}

// 轮询指令
static void fs_inrm303_pair_poll_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;

    // 可发送数据状态
    if ((fs_inrm303->data.status.status != FS_INRM303_STATUS_SYNCED) || fs_inrm303_is_ready(&fs_inrm303->data.ready) == false)
    {
        fs_inrm303->timestamp.tx = TIMESTAMP_US_GET(); // 重新计时
    }

    if (fs_inrm303->data.status.status == FS_INRM303_STATUS_PAIRING ||
        fs_inrm303->data.status.status == FS_INRM303_STATUS_SYNCED)
    {
        fs_inrm303->timestamp.pair = 0; // 清除时间戳
    }

    if (fs_inrm303->timestamp.pair != 0)
    {
        if (is_timeout(fs_inrm303->timestamp.pair, 500000)) // 500ms 重发
        {
            if (fs_inrm303->data.pair_config.Version == 1)
            {
                fs_inrm303_pair_v1(fs_inrm303); // 重发
            }
            else
            {
                fs_inrm303_pair_v0(fs_inrm303); // 重发
            }
            fs_inrm303->timestamp.pair = TIMESTAMP_US_GET();
        }
    }

    if (fs_inrm303->timestamp.tx != 0)
    {
        if (is_timeout(fs_inrm303->timestamp.tx, 50000)) // 50ms 发送一次
        {
            // 默认是实时数据
            fs_inrm303->data.tx_data.channel_size = fs_inrm303->cfg.channel_size;
            fs_inrm303->data.tx_data.is_real_data = 0x01;

            // 发送实时数据
            fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NO_RETURN, FS_INRM303_CID_TX_DATA),
                                  (uint8_t *)&fs_inrm303->data.tx_data, FS_INRM303_DATA_LENGTH_TX_DATA(fs_inrm303->cfg.channel_size));
            fs_inrm303->timestamp.tx = TIMESTAMP_US_GET();
        }
    }
}

static void fs_inrm303_init_cmd_pair(fs_inrm303_t *fs_inrm303)
{
    memset(&fs_inrm303->data.pair_config, 0, sizeof(fs_inrm303->data.pair_config));
    dds_subcribe(&fs_inrm303->POLL, DDS_PRIORITY_NORMAL, fs_inrm303_pair_poll_cb, NULL);
}

/**
 * @file rf_test.c
 * @author WittXie
 * @brief 测试模式
 * @version 0.1
 * @date 2024-12-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../fs_inrm303.h"

// 进入测试模式
void fs_inrm303_send_cmd_rf_test(fs_inrm303_t *fs_inrm303, uint8_t power, eLNK_TestType type, int32_t channel)
{
    fs_inrm303->data.rf_test.rf_test = 1;
    fs_inrm303->data.rf_test.ChannelNb = channel;
    fs_inrm303->data.rf_test.RFPower = power;
    fs_inrm303->data.rf_test.Type = type;
    fs_inrm303->data.rf_test.PAOn = true;
    fs_inrm303->data.rf_test.Antenna = LNK_ANTENNA_BOTH;
    fs_inrm303->data.rf_test.TestTXPayloadLength = 30;
    fs_inrm303->data.rf_test.TestRXPayloadLength = 30;
    fs_inrm303->data.rf_test.FeedbackRatio = 3;
    fs_inrm303->data.rf_test.EMIStandard = LNK_ES_CE;
    fs_inrm303->data.rf_test.BroadcastRXID = true;
    fs_inrm303->data.rf_test.PHYConfig.PacketType = PACKET_TYPE_PHY_SX1280_LORA;
    fs_inrm303->data.rf_test.PHYConfig.PacketParams.LoRa_Ranging.PreambleLengthMantissa = 12;
    fs_inrm303->data.rf_test.PHYConfig.PacketParams.LoRa_Ranging.PreambleLengthExponent = 0;
    fs_inrm303->data.rf_test.PHYConfig.PacketParams.LoRa_Ranging.HeaderMode = PHY_SX1280_LORA_IMPLICIT_HEADER;
    fs_inrm303->data.rf_test.PHYConfig.PacketParams.LoRa_Ranging.PayloadLength = 0;
    fs_inrm303->data.rf_test.PHYConfig.PacketParams.LoRa_Ranging.CRCMode = PHY_SX1280_LORA_CRC_ON;
    fs_inrm303->data.rf_test.PHYConfig.PacketParams.LoRa_Ranging.IQMode = PHY_SX1280_LORA_IQ_NORMAL;
    fs_inrm303->data.rf_test.PHYConfig.PacketParams.LoRa_Ranging.Reserved1 = 0;
    fs_inrm303->data.rf_test.PHYConfig.PacketParams.LoRa_Ranging.Reserved2 = 0;
    fs_inrm303->data.rf_test.PHYConfig.ModulationParams.LoRa_Ranging.SpreadingFactor = PHY_SX1280_LORA_SF5;
    fs_inrm303->data.rf_test.PHYConfig.ModulationParams.LoRa_Ranging.Bandwidth = PHY_SX1280_LORA_BW_0800;
    fs_inrm303->data.rf_test.PHYConfig.ModulationParams.LoRa_Ranging.CodingRate = PHY_SX1280_LORA_CR_LI_4_5;

    fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_RF_TEST), // 需要参数应答
                          (uint8_t *)&fs_inrm303->data.rf_test,
                          FS_INRM303_DATA_LENGTH_RF_TEST);

    fs_inrm303->timestamp.rf_test = TIMESTAMP_US_GET(); // 记录时间戳
}

// 轮询指令
static void fs_inrm303_rf_test_poll_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;

    if (fs_inrm303->timestamp.rf_test == 0)
    {
        // 不需要应答或者已经应答
        return;
    }

    if (is_timeout(fs_inrm303->timestamp.rf_test, 500000))
    {
        fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM, FS_INRM303_CID_RF_TEST), // 需要参数应答
                              (uint8_t *)&fs_inrm303->data.rf_test,
                              FS_INRM303_DATA_LENGTH_RF_TEST);
        fs_inrm303->timestamp.rf_test = TIMESTAMP_US_GET();
    }
}

// 监听指令
static void fs_inrm303_rf_test_received_cb(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    fs_inrm303_t *fs_inrm303 = (fs_inrm303_t *)device;
    fs_inrm303_received_t *received = (fs_inrm303_received_t *)arg;

    uint32_t cmd = received->cmd;
    uint8_t *data = received->data;
    uint32_t data_length = received->data_length;

    if (cmd != FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_PARAM, FS_INRM303_CID_RF_TEST))
    {
        return;
    }
    fs_inrm303->timestamp.rf_test = 0; // 清除时间戳
}

static void fs_inrm303_init_cmd_rf_test(fs_inrm303_t *fs_inrm303)
{
    memset(&fs_inrm303->data.rf_test, 0, sizeof(fs_inrm303->data.rf_test));

    dds_subcribe(&fs_inrm303->RECEIVED, DDS_PRIORITY_NORMAL, fs_inrm303_rf_test_received_cb, NULL);
    dds_subcribe(&fs_inrm303->POLL, DDS_PRIORITY_NORMAL, fs_inrm303_rf_test_poll_cb, NULL);
}

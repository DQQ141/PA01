/**
 * @file fs_inrm303_define.h
 * @author WittXie
 * @brief
 * @version 0.1
 * @date 2024-12-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#pragma pack(1) // 设置对齐方式为 1 字节

#define MAX_RF_CHANNELS_NUMBER 18
#define SES_NPT_NB_MAX_PORTS 4
#define SES_NB_MAX_CHANNELS 32

// END | Address | Frame Number | Frame Type | Protocol ID | DATA0~DATAn | CHECKSUM | END

// 帧头帧尾
#define FS_INRM303_END 0xC0     // 帧头或帧尾
#define FS_INRM303_ESC 0xDB     // 转义字符
#define FS_INRM303_ESC_END 0xDC // 转义后的帧尾, 用来转义END
#define FS_INRM303_ESC_ESC 0xDD // 转义后的转义字符，用来转义ESC

// 地址
#define FS_INRM303_ADDR_MAIN 0x01 // 遥控器地址
#define FS_INRM303_ADDR_RF 0x05   // RF地址

// 指令类型
typedef enum : uint8_t
{
    FS_INRM303_TYPE_READ = 0x01,                    // 读取
    FS_INRM303_TYPE_WRITE_NEED_RETURN_PARAM = 0x02, // 写入，接受端需要带参应答
    FS_INRM303_TYPE_WRITE_NEED_RETURN_NONE = 0x03,  // 写入，接受端需要无参应答
    FS_INRM303_TYPE_WRITE_NO_RETURN = 0x05,         // 单向发送，无需应答
    FS_INRM303_TYPE_ACK_PARAM = 0x10,               // 带参数的应答
    FS_INRM303_TYPE_ACK_NONE = 0x20,                // 无参数的应答
} fs_inrm303_type_enum_t;

// 指令ID
typedef enum : uint8_t
{
    FS_INRM303_CID_READY = 0x01,             // 就绪
    FS_INRM303_CID_STATUS = 0x02,            // 状态
    FS_INRM303_CID_MODE = 0x03,              // 模式设置
    FS_INRM303_CID_PAIR_PARAM_CONFIG = 0x04, // 对码参数配置
    FS_INRM303_CID_PAIR_PARAM_READ = 0x06,   // 对码参数读取
    FS_INRM303_CID_TX_DATA = 0x07,           // 发射机实时数据
    FS_INRM303_CID_RX_DATA = 0x09,           // 接收机回传实时数据
    FS_INRM303_CID_MAX_POWER = 0x10,         // 最大功率
    FS_INRM303_CID_BUZZER_SET = 0x12,        // 蜂鸣器设置
    FS_INRM303_CID_COMMAND = 0x0C,           // 命令指令
    FS_INRM303_CID_COMMAND_RESULT = 0x0D,    // 返回命令结果
    FS_INRM303_CID_RF_TEST = 0x0E,           // 射频测试
    FS_INRM303_CID_VERSION_INFO = 0x20,      // 版本信息
    FS_INRM303_CID_MODEL_SET = 0x2F,         // 模型设置
    FS_INRM303_CID_START_UPDATE = 0x30,      // 启动更新
    FS_INRM303_CID_REQUEST_FIRMWARE = 0x31,  // 请求固件
    FS_INRM303_CID_UPDATE_RESULT = 0x32,     // 更新结果
    FS_INRM303_CID_UPDATE_INFO = 0x33,       // 更新信息
} fs_inrm303_cid_enum_t;

// 状态
typedef enum : uint8_t
{
    FS_INRM303_STATUS_HARDWARE_ERROR = 0x01,                 // 硬件错误
    FS_INRM303_STATUS_PAIRING = 0x02,                        // 对码中
    FS_INRM303_STATUS_SYNC = 0x03,                           // 同步中
    FS_INRM303_STATUS_SYNCED = 0x04,                         // 已同步
    FS_INRM303_STATUS_STANDBY = 0x05,                        // 待机
    FS_INRM303_STATUS_RF_HEAD_WAITING_UPDATE = 0x06,         // 等待更新
    FS_INRM303_STATUS_RF_HEAD_UPDATING = 0x07,               // 更新中
    FS_INRM303_STATUS_RF_HEAD_UPDATE_RECEIVER = 0x08,        // 更新接收机
    FS_INRM303_STATUS_RF_HEAD_UPDATE_RECEIVER_FAILED = 0x09, // 更新接收机失败
    FS_INRM303_STATUS_RF_HEAD_RF_TEST = 0x0A,                // 射频测试
    FS_INRM303_STATUS_HARDWARE_TEST = 0xFF,                  // 硬件测试
} fs_inrm303_status_enum_t;

// 模式
typedef enum : uint8_t
{
    FS_INRM303_MODE_STANDBY = 0x01,  // 进入待机模式(进入待机模式后不能再发送实时数据（如通道值），否则会进入正常通信模式)
    FS_INRM303_MODE_PAIRING = 0x02,  // 进入对码模式(对码成功后自动进入正常通信状态，单向除外)
    FS_INRM303_MODE_NORMAL = 0x03,   // 进入正常通信模式
    FS_INRM303_MODE_UPDATE = 0x04,   // 进入更新接收机模式(更新完成自动进入待机状态，失败进入无线更新接收机失败状态)
    FS_INRM303_MODE_HARDWARE = 0x55, // 进入硬件测试模式（内部）
} fs_inrm303_mode_enum_t;

// 指令获取
#define FS_INRM303_CMD_GET(_type, _cid) ((((_type) & 0xFF) << 8) | (_cid & 0xFF))

typedef enum : uint8_t
{
    LNK_TT_CARRIER_ONLY,
    LNK_TT_SINGLE_CHANNEL,
    LNK_TT_WIDE_HOPPING,
    LNK_TT_FREQUENCY_SCAN
} eLNK_TestType;
typedef enum : uint8_t
{
    LNK_ANTENNA_BOTH,
    LNK_ANTENNA_1,
    LNK_ANTENNA_2,
} eLNK_Antenna;
typedef enum : uint8_t
{
    LNK_ES_FREE,
    LNK_ES_CE,
    LNK_ES_FCC
} eLNK_EMIStandard;

typedef enum : uint8_t
{
    SES_ANALOG_OUTPUT_PWM,
    SES_ANALOG_OUTPUT_PPM
} eSES_PA_SetAnalogOutput;

typedef enum : uint8_t
{
    CLASSIC_FLCR1_18CH = 0,
    CLASSIC_FLCR6_10CH,
    ROUTINE_FLCR1_18CH,
    ROUTINE_FLCR6_8CH,
    ROUTINE_LORA_12CH
} eDATA_PHYMODE;

typedef enum : uint8_t
{
    EB_BT_IBUS1,
    EB_BT_IBUS2,
    EB_BT_SBUS1
} eEB_BusType;

typedef enum : uint8_t
{
    SES_NPT_PWM,
    SES_NPT_PPM,
    SES_NPT_SBUS,
    SES_NPT_IBUS1_IN,
    SES_NPT_IBUS1_OUT,
    SES_NPT_IBUS2,
    SES_NPT_IBUS2_HUB_PORT,
    SES_NPT_WSTX,
    SES_NPT_WSRX,
    SES_NPT_NONE = 0xFF
} eSES_NewPortType;

typedef struct
{
    unsigned short Frequency : 15;   // From 50 to 400Hz
    unsigned short Synchronized : 1; // 1=Synchronize the PWM output to the RF cycle (lower latency but unstable frequency)
} sSES_PWMFrequencyV0;

typedef struct
{
    unsigned short PWMFrequencies[SES_NB_MAX_CHANNELS]; // One unsigned short per channel
    unsigned long Synchronized;                         // 1 bit per channel, 32 channels total
} sSES_PWMFrequenciesAPPV1;

typedef struct
{
    unsigned char Version;
    eLNK_EMIStandard EMIStandard;
    unsigned char IsTwoWay;
    eDATA_PHYMODE PhyMode;
    unsigned char SignalStrengthRCChannelNb; // 0xFF if not used
    unsigned short FailsafeTimeout;          // in unit of ms
    signed short FailSafe[MAX_RF_CHANNELS_NUMBER];
    unsigned char FailsafeOutputMode; // sSES_CA_SetIsFailsafeInNoOutputMode
    sSES_PWMFrequencyV0 PWMFrequency;
    eSES_PA_SetAnalogOutput AnalogOutput;
    eEB_BusType ExternalBusType;
} sDATA_ConfigV0;

typedef struct
{
    unsigned char Version;
    eLNK_EMIStandard EMIStandard;
    unsigned char IsTwoWay;
    eDATA_PHYMODE PhyMode;
    unsigned char SignalStrengthRCChannelNb; // 0xFF if not used
    unsigned short FailsafeTimeout;          // in unit of ms
    signed short FailSafe[MAX_RF_CHANNELS_NUMBER];
    unsigned char FailsafeOutputMode; // sSES_CA_SetIsFailsafeInNoOutputMode
    eSES_NewPortType NewPortTypes[SES_NPT_NB_MAX_PORTS];
    sSES_PWMFrequenciesAPPV1 PWMFrequenciesV1;
} sDATA_ConfigV1;

typedef enum : uint8_t
{
    PACKET_TYPE_PHY_SX1280_GFSK = 0x00,
    PACKET_TYPE_PHY_SX1280_LORA,
    PACKET_TYPE_PHY_SX1280_RANGING,
    PACKET_TYPE_PHY_SX1280_FLRC
} ePHY_SX1280_PacketType;

typedef enum : uint8_t
{
    PHY_SX1280_GFSK_BR_2_000_BW_2_4 = 0x04,
    PHY_SX1280_GFSK_BR_1_600_BW_2_4 = 0x28,
    PHY_SX1280_GFSK_BR_1_000_BW_2_4 = 0x4C,
    PHY_SX1280_GFSK_BR_1_000_BW_1_2 = 0x45,
    PHY_SX1280_GFSK_BR_0_800_BW_2_4 = 0x70,
    PHY_SX1280_GFSK_BR_0_800_BW_1_2 = 0x69,
    PHY_SX1280_GFSK_BR_0_500_BW_1_2 = 0x8D,
    PHY_SX1280_GFSK_BR_0_500_BW_0_6 = 0x86,
    PHY_SX1280_GFSK_BR_0_400_BW_1_2 = 0xB1,
    PHY_SX1280_GFSK_BR_0_400_BW_0_6 = 0xAA,
    PHY_SX1280_GFSK_BR_0_250_BW_0_6 = 0xCE,
    PHY_SX1280_GFSK_BR_0_250_BW_0_3 = 0xC7,
    PHY_SX1280_GFSK_BR_0_125_BW_0_3 = 0xEF,
} ePHY_SX1280_GFSK_BitrateBandwidth;

typedef enum : uint8_t
{
    PHY_SX1280_GFSK_MOD_IND_0_35 = 0,
    PHY_SX1280_GFSK_MOD_IND_0_50 = 1,
    PHY_SX1280_GFSK_MOD_IND_0_75 = 2,
    PHY_SX1280_GFSK_MOD_IND_1_00 = 3,
    PHY_SX1280_GFSK_MOD_IND_1_25 = 4,
    PHY_SX1280_GFSK_MOD_IND_1_50 = 5,
    PHY_SX1280_GFSK_MOD_IND_1_75 = 6,
    PHY_SX1280_GFSK_MOD_IND_2_00 = 7,
    PHY_SX1280_GFSK_MOD_IND_2_25 = 8,
    PHY_SX1280_GFSK_MOD_IND_2_50 = 9,
    PHY_SX1280_GFSK_MOD_IND_2_75 = 10,
    PHY_SX1280_GFSK_MOD_IND_3_00 = 11,
    PHY_SX1280_GFSK_MOD_IND_3_25 = 12,
    PHY_SX1280_GFSK_MOD_IND_3_50 = 13,
    PHY_SX1280_GFSK_MOD_IND_3_75 = 14,
    PHY_SX1280_GFSK_MOD_IND_4_00 = 15,
} ePHY_SX1280_GFSK_ModulationIndex;

typedef enum : uint8_t
{
    PHY_SX1280_MOD_SHAPING_BT_OFF = 0x00, //! No filtering
    PHY_SX1280_MOD_SHAPING_BT_1_0 = 0x10,
    PHY_SX1280_MOD_SHAPING_BT_0_5 = 0x20,
} ePHY_SX1280_ModulationShaping;

typedef struct
{
    ePHY_SX1280_GFSK_BitrateBandwidth BitrateBandwidth;
    ePHY_SX1280_GFSK_ModulationIndex ModulationIndex;
    ePHY_SX1280_ModulationShaping ModulationShaping;
} sPHY_SX1280_ModulationParamsGFSK;

typedef enum : uint8_t
{
    PHY_SX1280_FLRC_BR_1_300_BW_1_2 = 0x45,
    PHY_SX1280_FLRC_BR_1_040_BW_1_2 = 0x69,
    PHY_SX1280_FLRC_BR_0_650_BW_0_6 = 0x86,
    PHY_SX1280_FLRC_BR_0_520_BW_0_6 = 0xAA,
    PHY_SX1280_FLRC_BR_0_325_BW_0_3 = 0xC7,
    PHY_SX1280_FLRC_BR_0_260_BW_0_3 = 0xEB,
} ePHY_SX1280_FLRC_BitrateBandwidth;

typedef enum : uint8_t
{
    PHY_SX1280_FLRC_CR_1_2 = 0x00,
    PHY_SX1280_FLRC_CR_3_4 = 0x02,
    PHY_SX1280_FLRC_CR_1_0 = 0x04,
} ePHY_SX1280_FLRC_CodingRate;

typedef struct
{
    ePHY_SX1280_FLRC_BitrateBandwidth BitrateBandwidth;
    ePHY_SX1280_FLRC_CodingRate CodingRate;
    ePHY_SX1280_ModulationShaping ModulationShaping;
} sPHY_SX1280_ModulationParamsFLRC;

typedef enum : uint8_t
{
    PHY_SX1280_LORA_SF5 = 0x50,
    PHY_SX1280_LORA_SF6 = 0x60,
    PHY_SX1280_LORA_SF7 = 0x70,
    PHY_SX1280_LORA_SF8 = 0x80,
    PHY_SX1280_LORA_SF9 = 0x90,
    PHY_SX1280_LORA_SF10 = 0xA0,
    PHY_SX1280_LORA_SF11 = 0xB0,
    PHY_SX1280_LORA_SF12 = 0xC0,
} ePHY_SX1280_LoRa_SpreadingFactor;

typedef enum : uint8_t
{
    PHY_SX1280_LORA_BW_0200 = 0x34,
    PHY_SX1280_LORA_BW_0400 = 0x26,
    PHY_SX1280_LORA_BW_0800 = 0x18,
    PHY_SX1280_LORA_BW_1600 = 0x0A,
} ePHY_SX1280_LoRa_Bandwidth;

typedef enum : uint8_t
{
    PHY_SX1280_LORA_CR_4_5 = 0x01,
    PHY_SX1280_LORA_CR_4_6 = 0x02,
    PHY_SX1280_LORA_CR_4_7 = 0x03,
    PHY_SX1280_LORA_CR_4_8 = 0x04,
    PHY_SX1280_LORA_CR_LI_4_5 = 0x05,
    PHY_SX1280_LORA_CR_LI_4_6 = 0x06,
    PHY_SX1280_LORA_CR_LI_4_7 = 0x07,
} ePHY_SX1280_LoRa_CodingRate;

typedef struct
{
    ePHY_SX1280_LoRa_SpreadingFactor SpreadingFactor;
    ePHY_SX1280_LoRa_Bandwidth Bandwidth;
    ePHY_SX1280_LoRa_CodingRate CodingRate;
} sPHY_SX1280_ModulationParamsLoRa_Ranging;

typedef union
{
    sPHY_SX1280_ModulationParamsGFSK GFSK;
    sPHY_SX1280_ModulationParamsFLRC FLRC;
    sPHY_SX1280_ModulationParamsLoRa_Ranging LoRa_Ranging;
} sPHY_SX1280_ModulationParams;

typedef enum : uint8_t
{
    PHY_SX1280_GFSK_PREAMBLE_LENGTH_04_BITS = 0x00,
    PHY_SX1280_GFSK_PREAMBLE_LENGTH_08_BITS = 0x10,
    PHY_SX1280_GFSK_PREAMBLE_LENGTH_12_BITS = 0x20,
    PHY_SX1280_GFSK_PREAMBLE_LENGTH_16_BITS = 0x30,
    PHY_SX1280_GFSK_PREAMBLE_LENGTH_20_BITS = 0x40,
    PHY_SX1280_GFSK_PREAMBLE_LENGTH_24_BITS = 0x50,
    PHY_SX1280_GFSK_PREAMBLE_LENGTH_28_BITS = 0x60,
    PHY_SX1280_GFSK_PREAMBLE_LENGTH_32_BITS = 0x70,
} ePHY_SX1280_GFSK_PreambleLength;

typedef enum : uint8_t
{
    PHY_SX1280_GFSK_SYNCWORD_LENGTH_1_BYTE = 0x00,
    PHY_SX1280_GFSK_SYNCWORD_LENGTH_2_BYTE = 0x02,
    PHY_SX1280_GFSK_SYNCWORD_LENGTH_3_BYTE = 0x04,
    PHY_SX1280_GFSK_SYNCWORD_LENGTH_4_BYTE = 0x06,
    PHY_SX1280_GFSK_SYNCWORD_LENGTH_5_BYTE = 0x08,
} ePHY_SX1280_GFSK_SyncWordLength;

typedef enum : uint8_t
{
    PHY_SX1280_GFSK_FLRC_SYNCWORD_OFF = 0x00,
    PHY_SX1280_GFSK_FLRC_SYNCWORD_1 = 0x10,
    PHY_SX1280_GFSK_FLRC_SYNCWORD_2 = 0x20,
    PHY_SX1280_GFSK_FLRC_SYNCWORD_1_2 = 0x30,
    PHY_SX1280_GFSK_FLRC_SYNCWORD_3 = 0x40,
    PHY_SX1280_GFSK_FLRC_SYNCWORD_1_3 = 0x50,
    PHY_SX1280_GFSK_FLRC_SYNCWORD_2_3 = 0x60,
    PHY_SX1280_GFSK_FLRC_SYNCWORD_1_2_3 = 0x70,
} ePHY_SX1280_GFSK_FLRC_SyncWordCombination;

typedef enum : uint8_t
{
    PHY_SX1280_GFSK_FLRC_PACKET_FIXED_LENGTH = 0x00,
    PHY_SX1280_GFSK_FLRC_PACKET_VARIABLE_LENGTH = 0x20,
} ePHY_SX1280_GFSK_FLRC_PacketLengthMode;

typedef enum : uint8_t
{
    PHY_SX1280_GFSK_CRC_OFF = 0x00,
    PHY_SX1280_GFSK_CRC_1_BYTES = 0x10,
    PHY_SX1280_GFSK_CRC_2_BYTES = 0x20,
} ePHY_SX1280_GFSK_CRCLength;

typedef enum : uint8_t
{
    PHY_SX1280_GFSK_WHITENING_ON = 0x00,
    PHY_SX1280_GFSK_WHITENING_OFF = 0x08,
} ePHY_SX1280_GFSK_WhiteningMode;

typedef struct
{
    ePHY_SX1280_GFSK_PreambleLength PreambleLength;
    ePHY_SX1280_GFSK_SyncWordLength SyncWordLength;
    ePHY_SX1280_GFSK_FLRC_SyncWordCombination SyncWordCombination;
    ePHY_SX1280_GFSK_FLRC_PacketLengthMode PacketLengthMode;
    unsigned char PayloadLength; // 0 to 255 bytes, ignored and copied from PayloadLength
    ePHY_SX1280_GFSK_CRCLength CRCLength;
    ePHY_SX1280_GFSK_WhiteningMode WhiteningMode;
} sPHY_SX1280_PacketParamsGFSK;

typedef enum : uint8_t
{
    PHY_SX1280_FLRC_PREAMBLE_LENGTH_08_BITS = 0x10,
    PHY_SX1280_FLRC_PREAMBLE_LENGTH_12_BITS = 0x20,
    PHY_SX1280_FLRC_PREAMBLE_LENGTH_16_BITS = 0x30,
    PHY_SX1280_FLRC_PREAMBLE_LENGTH_20_BITS = 0x40,
    PHY_SX1280_FLRC_PREAMBLE_LENGTH_24_BITS = 0x50,
    PHY_SX1280_FLRC_PREAMBLE_LENGTH_28_BITS = 0x60,
    PHY_SX1280_FLRC_PREAMBLE_LENGTH_32_BITS = 0x70,
} ePHY_SX1280_FLRC_PreambleLength;

typedef enum : uint8_t
{
    PHY_SX1280_FLRC_SYNC_NOSYNC = 0x00,        // 21 bits preamble
    PHY_SX1280_FLRC_SYNC_WORD_LEN_P32S = 0x04, // 21 bits preamble + 32 bits sync word
} ePHY_SX1280_FLRC_SyncWordLength;

typedef enum : uint8_t
{
    PHY_SX1280_FLRC_CRC_OFF = 0x00,
    PHY_SX1280_FLRC_CRC_1_BYTES = 0x10,
    PHY_SX1280_FLRC_CRC_2_BYTES = 0x20,
    PHY_SX1280_FLRC_CRC_3_BYTES = 0x30,
} ePHY_SX1280_FLRC_CRCLength;

typedef enum : uint8_t
{
    PHY_SX1280_FLRC_WHITENING_OFF = 0x08,
} ePHY_SX1280_FLRC_WhiteningMode;

typedef struct
{
    ePHY_SX1280_FLRC_PreambleLength PreambleLength;
    ePHY_SX1280_FLRC_SyncWordLength SyncWordLength;
    ePHY_SX1280_GFSK_FLRC_SyncWordCombination SyncWordCombination;
    ePHY_SX1280_GFSK_FLRC_PacketLengthMode PacketLengthMode;
    unsigned char PayloadLength; // 6 to 127 bytes, ignored and copied from PayloadLength
    ePHY_SX1280_FLRC_CRCLength CRCLength;
    ePHY_SX1280_FLRC_WhiteningMode WhiteningMode;
} sPHY_SX1280_PacketParamsFLRC;

typedef enum : uint8_t
{
    PHY_SX1280_LORA_EXPLICIT_HEADER = 0x00,
    PHY_SX1280_LORA_IMPLICIT_HEADER = 0x80,
} ePHY_SX1280_LoRa_HeaderMode;

typedef enum : uint8_t
{
    PHY_SX1280_LORA_CRC_ON = 0x20,
    PHY_SX1280_LORA_CRC_OFF = 0x00,
} ePHY_SX1280_LoRa_CRCMode;

typedef enum : uint8_t
{
    PHY_SX1280_LORA_IQ_NORMAL = 0x40,
    PHY_SX1280_LORA_IQ_INVERTED = 0x00,
} ePHY_SX1280_LoRa_IQMode;

typedef struct
{
    unsigned char PreambleLengthMantissa : 4;
    unsigned char PreambleLengthExponent : 4;
    ePHY_SX1280_LoRa_HeaderMode HeaderMode;
    unsigned char PayloadLength; // 1 to 255 bytes, ignored and copied from PayloadLength
    ePHY_SX1280_LoRa_CRCMode CRCMode;
    ePHY_SX1280_LoRa_IQMode IQMode;
    unsigned char Reserved1;
    unsigned char Reserved2;
} sPHY_SX1280_PacketParamsLoRa_Ranging;

typedef union
{
    sPHY_SX1280_PacketParamsGFSK GFSK;
    sPHY_SX1280_PacketParamsFLRC FLRC;
    sPHY_SX1280_PacketParamsLoRa_Ranging LoRa_Ranging;
} sPHY_SX1280_PacketParams;

typedef struct
{
    unsigned char CommandNb;
    sPHY_SX1280_ModulationParams Params;
} sPHY_SX1280_SetModulationParamsCommand;

typedef struct
{
    unsigned char CommandNb;
    sPHY_SX1280_PacketParams Params;
} sPHY_SX1280_SetPacketParamsCommand;

typedef struct
{
    unsigned char CommandNb;
    ePHY_SX1280_PacketType Type;
} sPHY_SX1280_SetPacketTypeCommand;
typedef struct
{
    unsigned char PhyTXPayloadLength;
    unsigned char PhyRXPayloadLength;
    uint8_t CarrierOnly : 1;
    uint8_t MeasureRSSI : 1;  // Measure RSSI if TRUE
    uint8_t AdaptiveMode : 1; // Adaptive frequency hopping mode
    uint8_t Reserved : 5;
    ePHY_SX1280_PacketType PacketType;
    sPHY_SX1280_PacketParams PacketParams;
    sPHY_SX1280_ModulationParams ModulationParams;
} sPHY_Config;

#pragma pack() // 恢复默认对齐方式

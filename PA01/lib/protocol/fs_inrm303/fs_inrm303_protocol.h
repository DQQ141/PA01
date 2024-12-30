/**
 * @file fs_inrm303_protocol.h
 * @author WittXie
 * @brief 富斯摇杆协议
 * @version 0.1
 * @date 2024-11-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "./../../algorithm/compare/compare.h" // 比较算法
#include "./fs_inrm303_define.h"               // 定义

#pragma pack(1) // 设置对齐方式为 1 字节

// 模块就绪
typedef struct __fs_inrm303_ready
{
    uint8_t equl_2_is_ready; // 状态, 这个状态只有两个值：0x01表示未就绪，0x02表示就绪
} fs_inrm303_ready_t;
static inline bool fs_inrm303_is_ready(fs_inrm303_ready_t *ready)
{
    return ready->equl_2_is_ready == 0x02;
}
#define FS_INRM303_DATA_LENGTH_READY 1
#define FS_INRM303_DATA_SET_READY(_ready, _array) \
    {                                             \
        (_ready)->equl_2_is_ready = _array[0];    \
    }

// 模块状态
typedef struct __fs_inrm303_status
{
    fs_inrm303_status_enum_t status; // 状态
} fs_inrm303_status_t;
#define FS_INRM303_DATA_LENGTH_STATUS 1
#define FS_INRM303_DATA_SET_STATUS(_status, _array) \
    {                                               \
        (_status)->status = _array[0];              \
    }

// 模块信息
typedef struct __fs_inrm303_info
{
    uint16_t company_id;           // 公司ID
    uint32_t tx_id;                // 发射机ID
    uint32_t rx_id;                // 接收机ID
    uint32_t product_id;           // 产品ID
    uint8_t hw_version[4];         // 硬件版本
    uint8_t bootloader_version[4]; // Bootloader版本
    uint8_t fw_version[4];         // 固件版本
    uint8_t rf_version[4];         // RF版本
} fs_inrm303_info_t;
#define FS_INRM303_DATA_LENGTH_INFO (sizeof(fs_inrm303_info_t)) // 兼容老代码，4字节对齐，这里不能直接用sizeof
#define FS_INRM303_DATA_SET_INFO(_info, _array)                                                                                \
    {                                                                                                                          \
        uint8_t *p = _array;                                                                                                   \
        (_info)->company_id = *(uint16_t *)(p), p += sizeof(uint16_t);                                                         \
        (_info)->tx_id = *(uint32_t *)(p), p += sizeof(uint32_t);                                                              \
        (_info)->rx_id = *(uint32_t *)(p), p += sizeof(uint32_t);                                                              \
        (_info)->product_id = *(uint32_t *)(p), p += sizeof(uint32_t);                                                         \
        memcpy((_info)->hw_version, p, sizeof((_info)->hw_version)), p += sizeof((_info)->hw_version);                         \
        memcpy((_info)->bootloader_version, p, sizeof((_info)->bootloader_version)), p += sizeof((_info)->bootloader_version); \
        memcpy((_info)->fw_version, p, sizeof((_info)->fw_version)), p += sizeof((_info)->fw_version);                         \
        memcpy((_info)->rf_version, p, sizeof((_info)->rf_version)), p += sizeof((_info)->rf_version);                         \
    }

// 模块模式设置
typedef struct __fs_inrm303_send_cmd_mode
{
    fs_inrm303_mode_enum_t mode; // 模式
} fs_inrm303_send_cmd_mode_t;
#define FS_INRM303_DATA_LENGTH_MODE 1

// 发射机实时数据
typedef struct __fs_inrm303_tx_data
{
    uint8_t is_real_data; // 是否是实时数据 0x01表示是实时数据，0x02表示失控保护
    uint8_t channel_size; // 通道数量
    int16_t channel[32];  // 通道值, 最大32个通道
} fs_inrm303_tx_data_t;
#define FS_INRM303_DATA_LENGTH_TX_DATA(_channel_size) (2 + (_channel_size) * sizeof(int16_t))

// 对码参数配置
typedef union __fs_inrm303_pair_config
{
    unsigned char Version;
    sDATA_ConfigV0 ConfigV0;
    sDATA_ConfigV1 ConfigV1;
} fs_inrm303_pair_config_t;
#define FS_INRM303_DATA_SET_PAIR_CONFIG(_pair_config, _array)           \
    {                                                                   \
        memcpy(_pair_config, _array, sizeof(fs_inrm303_pair_config_t)); \
    }

// 测试配置
typedef struct __fs_inrm303_rf_test
{
    uint8_t rf_test;        // 射频测试 0x01表示射频测试, 0x02表示接收机测试
    int32_t ChannelNb;      // If negative, represents the positive frequency offset from the beginning of the band in 100Hz increments
    uint16_t AuxiliaryTime; // Time inserted after each feedback for an auxiliary function, may be zero
    uint16_t RFPower;       // Power used when testing in unit of 0.25dBm
    uint8_t Type : 4;
    uint8_t Antenna : 2;
    uint8_t PAOn : 1;
    uint8_t BroadcastRXID : 1;   // If FALSE, the RX ID is not broadcasted to save 4 bytes on each side
    uint8_t TestTXPayloadLength; // Length of transmitter payload
    uint8_t TestRXPayloadLength; // Length of feedback (receiver) payload
    uint8_t FeedbackRatio;       // 0=1-way transmitter, 1=feedback every cycle, 2=feedback every 2 cycles... up to 15
    sPHY_Config PHYConfig;
    uint8_t RXTimeMargin; // Time margin before and after reception for both transmitter and receiver in units of 10us
    eLNK_EMIStandard EMIStandard;
} fs_inrm303_rf_test_t;
#define FS_INRM303_DATA_LENGTH_RF_TEST (sizeof(fs_inrm303_rf_test_t))
#define FS_INRM303_DATA_SET_RF_TEST(_rf_test, _array)           \
    {                                                           \
        memcpy(_rf_test, _array, sizeof(fs_inrm303_rf_test_t)); \
    }

typedef struct __fs_inrm303_rf_test_return
{
    uint8_t rf_test;         // 射频测试 0x01表示射频测试, 0x02表示接收机测试
    uint8_t test_is_succuss; // 测试成功 0x01表示成功, 0x02表示失败
} fs_inrm303_rf_test_return_t;
#define FS_INRM303_DATA_LENGTH_RF_TEST_RETURN 2
#define FS_INRM303_DATA_SET_RF_TEST_RETURN(_rf_test_return, _array)           \
    {                                                                         \
        memcpy(_rf_test_return, _array, sizeof(fs_inrm303_rf_test_return_t)); \
    }

typedef struct __fs_inrm303_cmd
{
    enum : uint16_t
    {
        GET_RECEIVER_ABILITY = 0x7015,
        SET_PPM_PWM_V0 = 0x7016,
        SET_PWM_FREQUENCY_V0 = 0x7017,
        SET_PWM_FREQUENCY_V1 = 0x7028,
        SET_IBUS_SBUS_V0 = 0x7018,
        SET_IBUS_IN_OUT_V0 = 0x7020,
        SET_CUSTOM_INTERFACE_TYPE_V1 = 0x7027,
        GET_RECEIVER_VERSION_INFO = 0x701F,
        BVD_CALIBRATION = 0x702C,
        SET_PPM_IBUS_SBUS_LOSS_PROTECTION = 0x702A,
        SET_LOSS_PROTECTION = 0x6011,
        SET_LOSS_PROTECTION_TIME = 0x6012,
        SET_RSSI_SIGNAL_OUTPUT_CHANNEL = 0x602B,
        SET_RF_OUTPUT_POWER = 0x2013,
    } code;

    enum : uint8_t
    {
        CODE_LENGTH_RECEIVER_ABILITY = 0,
        CODE_LENGTH_PPM_PWM_V0 = 1,
        CODE_LENGTH_PWM_FREQUENCY_V0 = 2,
        CODE_LENGTH_PWM_FREQUENCY_V1 = 32 + 3,
        CODE_LENGTH_IBUS_SBUS_V0 = 1,
        CODE_LENGTH_IBUS_IN_OUT_V0 = 1,
        CODE_LENGTH_CUSTOM_INTERFACE_TYPE_V1 = 4,
        CODE_LENGTH_RECEIVER_VERSION_INFO = 0,
        CODE_LENGTH_CALIBRATION = 8,
        CODE_LENGTH_PPM_IBUS_SBUS_LOSS_PROTECTION = 1,
        CODE_LENGTH_LOSS_PROTECTION_CLASSIC_FLCR1_18CH = 36,
        CODE_LENGTH_LOSS_PROTECTION_CLASSIC_FLCR6_10CH = 20,
        CODE_LENGTH_LOSS_PROTECTION_ROUTINE_FLCR6_8CH = 16,
        CODE_LENGTH_LOSS_PROTECTION_ROUTINE_LORA_12CH = 24,
        CODE_LENGTH_LOSS_PROTECTION_TIME = 2,
        CODE_LENGTH_RSSI_SIGNAL_OUTPUT_CHANNEL = 1,
        CODE_LENGTH_RF_OUTPUT_POWER = 2,
    } code_length;

    union
    {
        uint16_t power;   // 功率
        uint8_t data[36]; // 最多36个字节
    };

} fs_inrm303_excmd_t;
#define FS_INRM303_DATA_LENGTH_EXCMD(_code_length) (3 + (_code_length))

#pragma pack() // 恢复默认对齐方式

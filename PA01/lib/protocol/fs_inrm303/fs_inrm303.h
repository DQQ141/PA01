/**
 * @file fs_inrm303.h
 * @author WittXie
 * @brief RF模块驱动
 * @version 0.1
 * @date 2024-10-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 依赖
#include "./../../dds/dds.h"       // 订阅机制
#include "./../../time/time.h"     // 时间
#include "./fs_inrm303_protocol.h" // 协议

#ifndef TIMESTAMP_US_GET
#error "please define TIMESTAMP_US_GET"
#define TIMESTAMP_US_GET() 0
#endif

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

#ifndef WARN
#define WARN(_format, ...) ((void)0)
#endif

#ifndef ERROR
#define ERROR(_format, ...) ((void)0)
#endif

#ifndef PRINT_HEX
#define PRINT_HEX(_format, _data, _length, ...) ((void)0)
#endif

#ifndef SLEEP_MS
#define SLEEP_MS(_ms)
#endif

typedef struct __fs_inrm303_received
{
    uint32_t cmd;
    uint8_t *data;
    uint32_t data_length;
} fs_inrm303_received_t;

typedef struct __fs_inrm303_data
{
    // 协议数据
    fs_inrm303_status_t status;           // 状态
    fs_inrm303_ready_t ready;             // 就绪
    fs_inrm303_info_t info;               // 信息
    fs_inrm303_send_cmd_mode_t mode;      // 模式设置
    fs_inrm303_tx_data_t tx_data;         // 发射机实时数据
    fs_inrm303_pair_config_t pair_config; // 对码参数配置
    fs_inrm303_rf_test_t rf_test;         // 射频测试
    fs_inrm303_excmd_t excmd;             // 拓展指令
} fs_inrm303_data_t;

typedef struct __fs_inrm303
{
    // 参数
    struct
    {
        const char *name;     // 名称
        uint32_t try_cnt;     // 初始化尝试次数
        uint8_t channel_size; // 通道数量
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);                                               // 初始化
        void (*write)(uint32_t cmd, uint8_t *data, uint32_t data_length); // 写入
    } ops;

    // 协议数据
    fs_inrm303_data_t data;

    // 响应
    struct
    {
        uint64_t rf_test; // 射频测试时间戳
        uint64_t pair;    // 对码时间戳
        uint64_t tx;      // 发射数据时间戳
        uint64_t status;  // 状态时间戳
        uint64_t mode;    // 模式时间戳
        uint64_t excmd;   // 拓展指令时间戳
        uint64_t ready;   // 就绪时间戳
    } timestamp;

    // 标志
    union
    {
        uint8_t value;
        struct
        {
            bool is_inited : 1;  // 是否已初始化
            bool is_running : 1; // 是否正在运行
        };
    } flag;
    uint32_t init_try_cnt; // 初始化尝试次数
    uint8_t frame_cnt;     // 帧号

    // 订阅
    dds_topic_t RECEIVED; // 接收到数据
    dds_topic_t POLL;     // 轮询
    dds_topic_t INIT;     // 初始化
} fs_inrm303_t;

/**
 * @brief 初始化串联FS_INRM303
 *
 * @data_param fs_inrm303 指向fs_inrm303_t结构的指针
 */
void fs_inrm303_init(fs_inrm303_t *fs_inrm303);

/**
 * @brief 论询
 *
 * @data_param fs_inrm303 指向fs_inrm303_t结构的指针
 */
void fs_inrm303_poll(fs_inrm303_t *fs_inrm303);

/**
 * @brief 读取数据
 *
 * @data_param fs_inrm303 指向fs_inrm303_t结构的指针
 * @data_param cmd 命令
 * @data_param data 接收数据缓冲区
 * @data_param data_length 接收数据长度
 */
void fs_inrm303_read_hook(fs_inrm303_t *fs_inrm303, uint32_t cmd, uint8_t *data, uint32_t data_length);

/**
 * @brief 设备是否初始化
 *
 * @data_param fs_inrm303 指向fs_inrm303_t结构的指针
 * @return bool true为已初始化，false为未初始化
 */
#define fs_inrm303_is_inited(fs_inrm303) ((fs_inrm303)->flag.is_inited)

/**
 * @brief 设备是否正在运行
 *
 * @data_param fs_inrm303 指向fs_inrm303_t结构的指针
 * @return bool true为正在运行，false为未运行
 */
#define fs_inrm303_is_running(fs_inrm303) ((fs_inrm303)->flag.is_running)

/**
 * @brief 发送数据
 *
 * @data_param fs_inrm303 指向fs_inrm303_t结构的指针
 * @data_param _cmd 命令
 * @data_param _data 发送数据缓冲区
 * @data_param _data_length 发送数据长度
 */
#define fs_inrm303_send(fs_inrm303, _cmd, _data, _data_length)    \
    do                                                            \
    {                                                             \
        (fs_inrm303)->ops.write((_cmd), (_data), (_data_length)); \
    } while (0)

// 指令

/**
 * @brief 发送测试模式指令
 *
 * @param fs_inrm303 指向fs_inrm303_t结构的指针
 * @param power 功率
 * @param type 测试类型
 * @param channel 通道
 */
void fs_inrm303_send_cmd_rf_test(fs_inrm303_t *fs_inrm303, uint8_t power, eLNK_TestType type, int32_t channel);

/**
 * @brief 开始对码
 *
 * @param fs_inrm303 指向fs_inrm303_t结构的指针
 */
void fs_inrm303_pair_v1(fs_inrm303_t *fs_inrm303);

/**
 * @brief 开始对码
 *
 * @param fs_inrm303 指向fs_inrm303_t结构的指针
 */
void fs_inrm303_pair_v0(fs_inrm303_t *fs_inrm303);

/**
 * @brief 设置通道值
 *
 * @param fs_inrm303 指向fs_inrm303_t结构的指针
 * @param channel 通道
 * @param value 值
 *
 */
void fs_inrm303_chnnel_set(fs_inrm303_t *fs_inrm303, uint8_t channel, int16_t value);

/**
 * @brief 设置通道us值
 *
 * @param fs_inrm303 指向fs_inrm303_t结构的指针
 * @param channel 通道
 * @param us 值
 *
 */
void fs_inrm303_chnnel_us_set(fs_inrm303_t *fs_inrm303, uint8_t channel, int16_t us);

/**
 * @brief 更新实时数据
 *
 * @param fs_inrm303 指向fs_inrm303_t结构的指针
 */
static inline void fs_inrm303_update(fs_inrm303_t *fs_inrm303)
{
    // 读取实时数据
    fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_READ, FS_INRM303_CID_READY), NULL, 0);  // 获取就绪
    fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_READ, FS_INRM303_CID_STATUS), NULL, 0); // 获取状态
}

/**
 * @brief 模式设置
 *
 * @param fs_inrm303 指向fs_inrm303_t结构的指针
 * @param mode FS_INRM303_MODE_STANDBY
 * @param mode FS_INRM303_MODE_PAIRING
 * @param mode FS_INRM303_MODE_NORMAL
 * @param mode FS_INRM303_MODE_UPDATE
 * @param mode FS_INRM303_MODE_HARDWARE
 *
 */
void fs_inrm303_send_cmd_mode(fs_inrm303_t *fs_inrm303, fs_inrm303_mode_enum_t mode);

/**
 * @brief 拓展指令：设置高频头RF输出功率
 *
 * @param fs_inrm303 指向fs_inrm303_t结构的指针
 * @param power 功率, 单位：0.25dBm
 *
 * @note 该指令只在正常通信模式下有效，单位：0.25dBm
 */
void fs_inrm303_send_cmd_power(fs_inrm303_t *fs_inrm303, uint16_t power);

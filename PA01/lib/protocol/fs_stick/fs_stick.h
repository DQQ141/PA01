/**
 * @file fs_stick.h
 * @author WittXie
 * @brief 摇杆驱动
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
#include "./../../dds/dds.h"     // 订阅机制
#include "./fs_stick_protocol.h" // 协议

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

typedef struct __fs_stick_data
{
    // 协议数据
    fs_protocol_stick_hall_t hall;       // 霍尔值
    fs_protocol_stick_param_t param;     // 参数
    fs_protocol_stick_version_t version; // 版本
} fs_stick_data_t;

typedef struct __fs_stick
{
    // 参数
    struct
    {
        const char *name; // 名称
        float threshold;  // 最小变动阈值 0~1
        uint32_t try_cnt; // 初始化尝试次数
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);                                               // 初始化
        void (*write)(uint32_t cmd, uint8_t *data, uint32_t data_length); // 写入
    } ops;

    // 协议数据
    fs_stick_data_t data; // 数据

    // 处理后的数据
    struct
    {
        float x; // 摇杆x轴
        float y; // 摇杆y轴
    } stick[2];  // 摇杆

    struct
    {
        float x;     // 摇杆x轴
        float y;     // 摇杆y轴
    } last_stick[2]; // 摇杆影子值

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

    dds_topic_t CHANGED;           // 更新
    dds_topic_t TO_PRESS;          // 按下
    dds_topic_t TO_RELEASE;        // 释放
    dds_topic_t KEEP_LONG_PRESS;   // 保持按压
    dds_topic_t KEEP_LONG_RELEASE; // 保持释放

} fs_stick_t;

/**
 * @brief 初始化串联FS_STICK
 *
 * @data_param fs_stick 指向fs_stick_t结构的指针
 */
void fs_stick_init(fs_stick_t *fs_stick);

/**
 * @brief 论询
 *
 * @data_param fs_stick 指向fs_stick_t结构的指针
 */
void fs_stick_poll(fs_stick_t *fs_stick);

/**
 * @brief 读取数据
 *
 * @data_param fs_stick 指向fs_stick_t结构的指针
 * @data_param data 接收数据缓冲区
 * @data_param data_length 接收数据长度
 */
void fs_stick_read_hook(fs_stick_t *fs_stick, uint32_t cmd, uint8_t *data, uint32_t data_length);

/**
 * @brief 设备是否初始化
 *
 * @data_param fs_stick 指向fs_stick_t结构的指针
 * @return bool true为已初始化，false为未初始化
 */
#define fs_stick_is_inited(fs_stick) ((fs_stick)->flag.is_inited)

/**
 * @brief 设备是否正在运行
 *
 * @data_param fs_stick 指向fs_stick_t结构的指针
 * @return bool true为正在运行，false为未运行
 */
#define fs_stick_is_running(fs_stick) ((fs_stick)->flag.is_running)

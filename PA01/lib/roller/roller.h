/**
 * @file roller.h
 * @author WittXie
 * @brief 滚轮
 * @version 0.1
 * @date 2024-09-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

// 依赖
#include "./../dds/dds.h" // 订阅机制

// 通用接口
#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

#ifndef TIMESTAMP_US_GET
#error "please define TIMESTAMP_US_GET"
#define TIMESTAMP_US_GET() 0
#endif

typedef struct __roller
{
    // 参数
    struct
    {
        const char *name;   // 名称
        uint32_t filter_us; // 滤波时间，单位微秒
        uint8_t div;        // 分频因子
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);
        int32_t (*read)(void); // 读取步数，有方向
    } ops;

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

    int32_t step;             // 步数结果；有方向
    float original_step;      // 原始步数；有方向
    float speed;              // 速度
    uint64_t timestamp_start; // 起始时间戳
    uint64_t timestamp_end;   // 结束时间戳

    dds_topic_t CHANGED; // 数据有更新
} roller_t;

/**
 * @brief 初始化
 *
 * @param roller 设备指针
 */
void roller_init(roller_t *roller);

/**
 * @brief 轮询
 *
 * @param roller 设备指针
 */
void roller_poll(roller_t *roller);

/**
 * @brief 设备是否初始化
 *
 * @param roller 设备指针
 * @return bool true为已初始化，false为未初始化
 */
#define roller_is_inited(roller) ((roller)->flag.is_inited)

/**
 * @brief 设备是否正在运行
 *
 * @param roller 设备指针
 * @return bool true为正在运行，false为未运行
 */
#define roller_is_running(roller) ((roller)->flag.is_running)

/**
 * @file time.h
 * @author WittXie
 * @brief 时间换算工具
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

// 日期时间结构体
typedef struct __date
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t ms; // 毫秒
    uint16_t us; // 微秒
} vdate_t;

typedef struct __time_t
{
    struct
    {
        uint8_t timer_bits; // 计时器位数
    } cfg;

    struct
    {
        void (*init)(void);
        uint32_t (*read)(void);            // 读取定时器时间戳
        void (*write)(uint32_t timer_cnt); // 写入定时器时间戳
    } ops;

    uint64_t timestamp_letf;    // 时间戳高位（微秒为单位）
    uint64_t timestamp_startup; // 开机时间戳（微秒为单位）
} vtime_t;

/**
 * @brief 获取时间戳
 * @return uint64_t 获取us级时间戳
 */
uint64_t timestamp_us_get(vtime_t *time);

/**
 * @brief 时间钩子函数
 *
 * @param time 时间设备指针
 */
void time_hook(vtime_t *time);

/**
 * @brief 获取当前日期
 *
 * @param time 时间设备指针
 * @return 当前日期
 */
vdate_t current_date_get(vtime_t *time);

/**
 * @brief 设置当前日期
 *
 * @param time 时间设备指针
 * @param date 要设置的日期地址
 */
void current_date_set(vtime_t *time, const vdate_t *date);

/**
 * @brief 距离[_timestamp]后的[_timegap]时间是否已经超时
 * @param _timestamp 时间戳
 * @param _time_gap 时间间隔
 * @return 0,未超时  1,超时
 */
#define is_timeout(_timestamp, _timegap) (((uint64_t)(TIMESTAMP_US)) > (uint64_t)((uint64_t)(_timestamp) + (uint64_t)(_timegap)))

/**
 * @brief 时间间隔调用
 * @param _timegap_us 间隔时间
 * @param _do_something 要做的事, 用{}包起来
 */
#define time_invoke(_timegap_us, _do_something)      \
    {                                                \
        static volatile uint64_t last_timestamp = 0; \
        if (is_timeout(last_timestamp, _timegap_us)) \
        {                                            \
            _do_something;                           \
            last_timestamp = TIMESTAMP_US;           \
        }                                            \
    }

/**
 * @brief 时间花费计算
 * @param _do_something 要做的事, 用{}包起来
 * @return 花费的时间 单位:us
 */
#define time_spent(_do_something)                   \
    ({                                              \
        volatile uint64_t _last_timestamp = 0;      \
        _last_timestamp = TIMESTAMP_US;             \
        _do_something;                              \
        (uint64_t)(TIMESTAMP_US - _last_timestamp); \
    })

/**
 * @brief 延时函数
 * @param _us 单位:us
 */
#define delay_us(_us)                       \
    {                                       \
        volatile uint64_t timestamp;        \
        timestamp = TIMESTAMP_US;           \
        while (!is_timeout(timestamp, _us)) \
        {                                   \
        }                                   \
    }

/**
 * @brief 检查给定的年份是否为闰年
 *
 * @param year 要检查的年份
 * @return 如果是闰年，返回true；否则返回false
 */
bool time_is_leap_year(uint16_t year);

/**
 * @brief 获取月份的天数
 *
 * @param year 年份
 * @param month 月份
 * @return 月份的天数
 */
uint8_t get_days_in_month(uint16_t year, uint8_t month);

/**
 * @brief 将Unix时间戳转换为日期
 *
 * @param timestamp_us Unix时间戳（微秒为单位）
 * @return 日期信息
 */
vdate_t timestamp_to_date(uint64_t timestamp_us);

/**
 * @brief 获取编译日期
 *
 * @return 编译日期信息
 */
vdate_t build_date_get(void);

/**
 * @brief 将date转换为Unix时间戳
 *
 * @param date 日期信息
 * @return Unix时间戳（微秒为单位）
 */
uint64_t date_to_timestamp(const vdate_t *date);

/**
 * @brief 将Unix时间戳转换为数字
 *
 * @param date 日期信息
 * @return uint64_t  例如: 2008-08-08 12:22:33 => 20080808122233
 */
uint64_t date_to_num(const vdate_t *date);

// 加载其他文件
#include "./nop.h"

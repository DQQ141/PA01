/**
 * @file gpio.h
 * @author WittXie
 * @brief GPIO控制
 * @version 0.1
 * @date 2024-05-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// 依赖
#include "./../dds/dds.h" //DDS

// 通用接口
#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef TIMESTAMP_US_GET
#error "please define TIMESTAMP_US_GET"
#define TIMESTAMP_US_GET() 0
#endif

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

#ifndef ERROR
#define ERROR(_format, ...) ((void)0)
#endif

typedef struct __gpio gpio_t;
typedef struct __gpio
{
    // 参数
    struct
    {
        const char *name;
        uint32_t timegap_short_press; // 短按时间间隔
        uint32_t timegap_long_press;  // 长按时间间隔
        uint32_t value;               // 写入值, 不得为0
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);
        uint32_t (*read)(void);
        void (*write)(uint32_t value);
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

    struct
    {
        uint64_t start_point;   // 脉冲开始时间点
        uint64_t end_point;     // 结束时间点
        uint32_t valid_width;   // 有效电平宽度 单位：us
        uint32_t invalid_width; // 无效电平宽度 单位：us
        uint32_t count;         // 0无限
    } pulse;

    struct
    {
        uint64_t start_point;
        uint64_t end_point;
        uint32_t last_value;
        union
        {
            uint8_t status;
            struct
            {
                uint8_t keep : 1;
                uint8_t not_used : 7;
            } flag;
        };
    } button;

    dds_topic_t REVERSED;             // IO翻转
    dds_topic_t TO_VALID;             // 转变为有效电平
    dds_topic_t TO_INVALID;           // 转变为无效电平
    dds_topic_t PRESSED;              // 短按过
    dds_topic_t RELEASEED;            // 短抬起过
    dds_topic_t LONG_PRESSED;         // 长按过
    dds_topic_t LONG_RELEASED;        // 长抬过
    dds_topic_t KEEP_LONG_PRESS;      // 保持按下
    dds_topic_t KEEP_LONG_RELEASE;    // 保持释放
    dds_topic_t TO_KEEP_LONG_PRESS;   // 转变为保持按下
    dds_topic_t TO_KEEP_LONG_RELEASE; // 转变为保持释放
    dds_topic_t PULSE_FINISHED;       // 脉冲结束
    dds_topic_t CHANGED;              // 发生改变
} gpio_t;

/**
 * @brief 获取gpio的有效电平宽度 单位：us
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 *
 * @return uint32_t 有效电平宽度
 */
#define gpio_pulse_valid_width_read(_p_gpio) ((_p_gpio)->pulse.valid_width)

/**
 * @brief 获取gpio的无效电平宽度 单位：us
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 *
 * @return uint32_t 无效电平宽度
 */
#define gpio_pulse_invalid_width_read(_p_gpio) ((_p_gpio)->pulse.invalid_width)

/**
 * @brief 获取脉冲周期 单位：us
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 *
 * @return 一个脉冲的周期
 */
#define gpio_pulse_period_read(_p_gpio) (gpio_pulse_valid_width_read(_p_gpio) + gpio_pulse_invalid_width_read(_p_gpio))

/**
 * @brief 获取脉冲频率: 单位 Hz
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 *
 * @return uint32_t 频率
 */
#define gpio_pulse_freq_read(_p_gpio) (1000000lu / gpio_pulse_period_read(_p_gpio))

/**
 * @brief 获取脉冲占空比 0~1
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @return float 占空比
 */
#define gpio_pulse_duty_read(_p_gpio) (gpio_pulse_valid_width_read(_p_gpio) / gpio_pulse_period_read(_p_gpio))

/**
 * @brief 频率写入
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @param freq 要生成的脉冲信号的频率，单位为赫兹(Hz)。
 * @param duty 脉冲信号的占空比，取值范围0.0到1.0，表示脉冲的高电平时间与周期的比率。
 * @param count 脉冲数
 */
#define gpio_freq_write(_p_gpio, _freq, _duty, _count)                                     \
    {                                                                                      \
        if ((_p_gpio)->flag.is_inited == false)                                            \
        {                                                                                  \
            ERROR("[%s] gpio_freq_write failed, gpio is not inited", (_p_gpio)->cfg.name); \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            gpio_pulse_freq_write((_p_gpio), _freq, _duty);                                \
            gpio_pulse_count_write((_p_gpio), _count);                                     \
        }                                                                                  \
    }

/**
 * @brief 周期写入
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @param freq 要生成的脉冲信号的频率，单位为赫兹(Hz)。
 * @param period 脉冲信号的占空比，取值范围0.0到1.0，表示脉冲的高电平时间与周期的比率。
 * @param count 脉冲数
 */
#define gpio_period_write(_p_gpio, _period, _duty, _count)                                   \
    {                                                                                        \
        if ((_p_gpio)->flag.is_inited)                                             					 \
        {                                                                                     \
            gpio_pulse_period_write((_p_gpio), _period, _duty);                              \
            gpio_pulse_count_write((_p_gpio), _count);                                       \
        }                                                                                    \
    }

/**
 * @brief 读取GPIO的值
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 *
 * @retuen GPIO的值
 */
#define gpio_read(_p_gpio) ((_p_gpio)->ops.read())

/**
 * @brief GPIO翻转
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 *
 */
#define gpio_toggle(_p_gpio)                                                           \
    {                                                                                  \
        if ((_p_gpio)->flag.is_inited == false)                                        \
        {                                                                              \
            ERROR("[%s] gpio_toggle failed, gpio is not inited", (_p_gpio)->cfg.name); \
        }                                                                              \
        else                                                                           \
        {                                                                              \
            (gpio_write((_p_gpio), !gpio_read(_p_gpio)));                              \
        }                                                                              \
    }

/**
 * @brief 初始化
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 */
void gpio_init(gpio_t *gpio);

/**
 * @brief 轮询GPIO接口以检查和处理GPIO状态变化。
 *
 * 该函数用于周期性地检查GPIO的状态，以确保能够及时响应输入信号的变化。
 * 它通常会被调用在一个循环或者定时器中断中，以保持对GPIO状态的持续监控。
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 */
void gpio_poll(gpio_t *p_gpio);

/**
 * @brief 根据指定的频率和占空比写脉冲到GPIO。
 *
 * 该函数用于生成具有特定频率和占空比的脉冲信号，通过GPIO输出。
 * 它利用定时器中断或软件定时来控制脉冲的持续时间和间隔。
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @param freq 要生成的脉冲信号的频率，单位为赫兹(Hz)。
 * @param duty 脉冲信号的占空比，取值范围0.0到1.0，表示脉冲的高电平时间与周期的比率。
 */
void gpio_pulse_freq_write(gpio_t *p_gpio, float freq, float duty);

/**
 * @brief 直接设置脉冲的高电平宽度和低电平宽度。
 *
 * 该函数允许用户直接指定脉冲的高电平（有效）宽度和低电平（无效）宽度。
 * 它适用于需要非常精确控制脉冲宽度的应用场景。
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @param valid_width 脉冲信号的高电平宽度，单位为微秒(us)。
 * @param invalid_width 脉冲信号的低电平宽度，单位为微秒(us)。
 */
void gpio_pulse_width_write(gpio_t *p_gpio, uint32_t valid_width, uint32_t invalid_width);

/**
 * @brief 根据指定的周期和占空比写脉冲到GPIO。
 *
 * 该函数用于生成具有特定周期和占空比的脉冲信号，通过GPIO输出。
 * 它通过计算周期和占空比来确定高电平和低电平的持续时间。
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @param period 脉冲信号的周期，单位为微秒(us)。
 * @param duty 脉冲信号的占空比，取值范围0.0到1.0，表示脉冲的高电平时间与周期的比率。
 */
void gpio_pulse_period_write(gpio_t *p_gpio, uint32_t period, float duty);

/**
 * @brief 写脉冲次数
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @param count 脉冲的次数
 */
void gpio_pulse_count_write(gpio_t *gpio, uint32_t count);

/**
 * @brief GPIO有效值写入
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @param valid 写入有效值
 */
void gpio_write(gpio_t *gpio, bool valid);

/**
 * @brief GPIO设置值
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @param value 写入值
 */
void gpio_value_set(gpio_t *gpio, uint32_t value);

#define gpio_value_get(_gpio) ((_gpio)->cfg.value)
#define gpio_value_update(_gpio) gpio_write(_gpio, true)

/**
 * @brief GPIO写入值
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @param value 写入值
 */
void gpio_value_write(gpio_t *gpio, uint32_t value);

/**
 * @brief 设备是否初始化
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @return bool true为已初始化，false为未初始化
 */
#define gpio_is_inited(_gpio) ((_gpio)->flag.is_inited)

/**
 * @brief 设备是否正在运行
 *
 * @param gpio 指向gpio_t结构的指针，该结构包含了GPIO操作所需的接口和数据。
 * @return bool true为正在运行，false为未运行
 */
#define gpio_is_running(_gpio) ((_gpio)->flag.is_running)

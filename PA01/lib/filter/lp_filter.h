/**
 * @file lp_filter.h
 * @author WittXie
 * @brief 低通滤波器
 * @version 0.1
 * @date 2024-08-26
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <math.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

// 低通滤波器结构体
typedef struct
{
    float alpha;       // 滤波系数
    float prev_output; // 上一次的输出值
    float prev_input;  // 上一次的输入值
    float cutoff_freq; // 截止频率
    float sample_rate; // 采样率
    int order;         // 阶数
    bool is_first;     // 是否是第一次
} lp_filter_t;

/**
 * @brief 初始化低通滤波器
 * @param filter 滤波器结构体指针
 * @param cutoff_freq 截止频率
 * @param sample_rate 采样率
 * @param order 阶数
 */
void lp_filter_init(lp_filter_t *filter, float cutoff_freq, float sample_rate, uint16_t order);

/**
 * @brief 更新低通滤波器的输出值
 * @param filter 滤波器结构体指针
 * @param input 当前输入值
 * @return 滤波后的输出值
 */
float lp_filter(lp_filter_t *filter, float input);

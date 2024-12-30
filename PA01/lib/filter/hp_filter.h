/**
 * @file hp_filter.h
 * @author WittXie
 * @brief 高通滤波器
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

// 高通滤波器结构体
typedef struct
{
    float alpha;       // 滤波系数
    float prev_output; // 上一次的输出值
    float prev_input;  // 上一次的输入值
    float cutoff_freq; // 截止频率
    float sample_rate; // 采样率
} hp_filter_t;

/**
 * @brief 初始化高通滤波器
 * @param filter 滤波器结构体指针
 * @param cutoff_freq 截止频率
 * @param sample_rate 采样率
 */
void hp_filter_init(hp_filter_t *filter, float cutoff_freq, float sample_rate);

/**
 * @brief 更新高通滤波器的输出值
 * @param filter 滤波器结构体指针
 * @param input 当前输入值
 * @return 滤波后的输出值
 */
float hp_filter(hp_filter_t *filter, float input);

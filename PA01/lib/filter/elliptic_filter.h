/**
 * @file elliptic_filter.h
 * @author WittXie
 * @brief 椭圆滤波器
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

typedef struct
{
    // y[n]=b0⋅x[n]+b1⋅x[n−1]+b2⋅x[n−2]−a1⋅y[n−1]−a2⋅y[n−2]
    float a1, a2;     // 输出系数
    float b0, b1, b2; // 输入系数
} elliptic_filter_coeff_t;

// 椭圆滤波器结构体
typedef struct
{
    elliptic_filter_coeff_t coeff; // 滤波器系数
    float x1, x2;                  // 上两次的输入值
    float y1, y2;                  // 上两次的输出值
    float cutoff_freq;             // 截止频率
    float sample_rate;             // 采样率
    float ripple;                  // 通带波动
    float stopband_attenuation;    // 阻带衰减
} elliptic_filter_t;

/**
 * @brief 初始化椭圆滤波器
 * @param coeff 滤波器系数
 * @param filter 滤波器结构体指针
 * @param cutoff_freq 截止频率
 * @param sample_rate 采样率
 * @param ripple 通带波动
 * @param stopband_attenuation 阻带衰减
 */
void elliptic_filter_init(elliptic_filter_t *filter, elliptic_filter_coeff_t coeff, float cutoff_freq, float sample_rate, float ripple, float stopband_attenuation);

/**
 * @brief 更新椭圆滤波器的输出值
 * @param filter 滤波器结构体指针
 * @param input 当前输入值
 * @return 滤波后的输出值
 */
float elliptic_filter(elliptic_filter_t *filter, float input);

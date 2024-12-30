/**
 * @file kalman_filter.h
 * @author WittXie
 * @brief 卡尔曼滤波器
 * @version 0.1
 * @date 2024-08-26
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

// 卡尔曼滤波器结构体
typedef struct
{
    float x_est;  // 估计状态
    float p_est;  // 估计误差协方差
    float x_pred; // 预测状态
    float p_pred; // 预测误差协方差
    float k_gain; // 卡尔曼增益
    float q;      // 过程噪声协方差
    float r;      // 测量噪声协方差
} kalman_filter_t;

/**
 * @brief 初始化卡尔曼滤波器
 * @param filter 滤波器结构体指针
 * @param q 过程噪声协方差
 * @param r 测量噪声协方差
 */
void kalman_filter_init(kalman_filter_t *filter, float q, float r);

/**
 * @brief 更新卡尔曼滤波器的输出值
 * @param filter 滤波器结构体指针
 * @param input 当前测量值
 * @return 滤波后的输出值
 */
float kalman_filter(kalman_filter_t *filter, float input);

/**
 * @brief 自适应调整过程噪声协方差和测量噪声协方差
 *
 * @param filter 滤波器结构体指针
 * @param measurement_error 测量误差：当前测量值与滤波器输出值之差
 */
void kalman_filter_adapt(kalman_filter_t *filter, float measurement_error);

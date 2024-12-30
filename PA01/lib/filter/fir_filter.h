/**
 * @file fir_filter.h
 * @author WittXie
 * @brief FIR滤波器
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

// FIR滤波器结构体
typedef struct
{
    float *coeff;          // 滤波器系数
    float *buff;           // 输入缓冲区
    uint16_t buff_length;  // 缓冲区长度，缓存的长度必须大于输入的数据长度
    uint16_t coeff_length; // 系数长度
    uint16_t idx;          // 当前缓冲区索引
} fir_filter_t;

/**
 * @brief 初始化FIR滤波器
 * @param filter 滤波器结构体指针
 * @param coeff 滤波器系数数组
 * @param coeff_length 滤波器系数长度
 * @param buff 缓冲
 * @param buff_length 输入缓冲区长度
 */
void fir_filter_init(fir_filter_t *filter, float *coeff, uint16_t coeff_length, float *buff, uint16_t buff_length);

/**
 * @brief 更新FIR滤波器的输出值
 * @param filter 滤波器结构体指针
 * @param input 当前输入值, 输入次数 < buff_length
 * @return 滤波后的输出值
 */
float fir_filter(fir_filter_t *filter, float input);

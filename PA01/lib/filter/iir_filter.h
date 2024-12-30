/**
 * @file iir_filter.h
 * @author WittXie
 * @brief IIR滤波器
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

// IIR滤波器结构体
typedef struct
{
    float *fw_coeff;      // 分子系数 (feedforward)
    float *bk_coeff;      // 分母系数 (feedback)
    float *input_buff;    // 输入缓冲区
    float *output_buff;   // 输出缓冲区
    uint16_t fw_length;   // 分子系数长度
    uint16_t bk_length;   // 分母系数长度
    uint16_t buff_length; // 缓冲区长度
    uint16_t idx;         // 当前缓冲区索引
} iir_filter_t;

/**
 * @brief 初始化IIR滤波器
 * @param filter 滤波器结构体指针
 * @param fw_coeff 分子系数数组
 * @param fw_length 分子系数长度
 * @param bk_coeff 分母系数数组
 * @param bk_length 分母系数长度
 * @param input_buff 输入缓冲区
 * @param output_buff 输出缓冲区
 * @param buff_length 缓冲区长度
 */
void iir_filter_init(iir_filter_t *filter,
                     float *fw_coeff,
                     uint16_t fw_length,
                     float *bk_coeff,
                     uint16_t bk_length,
                     float *input_buff,
                     float *output_buff,
                     uint16_t buff_length);

/**
 * @brief 更新IIR滤波器的输出值
 * @param filter 滤波器结构体指针
 * @param input 当前输入值
 * @return 滤波后的输出值
 */
float iir_filter(iir_filter_t *filter, float input);

/**
 * @file fft.h
 * @author WittXie
 * @brief 通用FFT算法模块（浮点数版本，静态内存分配，迭代方法）
 * @version 0.1
 * @date 2024-08-29
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

/**
 * @brief 复数结构体
 */
typedef struct
{
    float real;
    float imag;
} complex_t;

/**
 * @brief FFT结构体
 */
typedef struct
{
    uint16_t size;     // FFT点数
    complex_t *input;  // 输入数据
    complex_t *output; // 输出数据
    bool inverse;      // 是否为逆FFT
} fft_t;

/**
 * @brief 初始化FFT
 * @param fft FFT结构体指针
 * @param size FFT点数
 * @param inverse 是否为逆FFT
 * @param input_buffer 输入数据缓冲区
 * @param output_buffer 输出数据缓冲区
 */
void fft_init(fft_t *fft, uint16_t size, bool inverse, complex_t *input_buffer, complex_t *output_buffer);

/**
 * @brief 执行FFT计算
 * @param fft FFT结构体指针
 */
void fft_execute(fft_t *fft);

/**
 * @brief 释放FFT结构体内存（静态内存分配，无需释放）
 * @param fft FFT结构体指针
 */
static inline void fft_deinit(fft_t *fft) { (void)fft; }
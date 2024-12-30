/**
 * @file fit.h
 * @author WittXie
 * @brief 拟合算法（最小二乘法）
 * @version 0.1
 * @date 2024-08-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

// typedef定义算法函数
typedef float (*fit_math_func)(float x);

typedef struct
{
    fit_math_func func;
    fit_math_func defunc;
    bool enable_r2;
} fit_cfg_t;

// 指数拟合参数结构体
typedef struct
{
    float a;  // 参数a
    float b;  // 参数b
    float r2; // r^2 值
} fit_params_t;

// 数据点结构体
typedef struct
{
    float x;
    float y;
} fit_data_t;

/**
 * @brief 预测函数
 *
 * @param cfg 含互逆函数对；例如 expf与logf
 * @param params 拟合参数
 * @param x 预测位置
 * @return float 预测值
 */
float fit_predict(fit_cfg_t *cfg, fit_params_t params, int x);

/**
 * @brief 指数拟合函数
 *
 * @param cfg 含互逆函数对；例如 expf与logf
 * @param data 数据点数组
 * @param n 数据点数量
 * @return 拟合参数
 */
fit_params_t fit_training(fit_cfg_t *cfg, fit_data_t *data, uint32_t n);

/**
 * @file adc_bsp.h
 * @author WittXie
 * @brief ADC驱动
 * @version 0.1
 * @date 2024-09-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

// 环境
#include "../bsp_env.h"

// 依赖
#include "./../log/log_bsp.h"
#include "./../os/os_bsp.h"
#include "./../time/time_bsp.h"

/**
 * @brief bsp初始化
 *
 */
void adc_bsp_init(void);

/**
 * @brief ADC是否在运行
 *
 * @return true 正在运行
 * @return false 未运行
 */
bool adc_is_running(void);

extern float g_adc_usb;        // USB电压
extern float g_adc_battery;    // 电池电压
extern float g_adc_pot_left;   // 左电位器电压
extern float g_adc_pot_right;  // 右电位器电压
extern float g_adc_sw_left;    // 左三段式开关
extern float g_adc_sw_right;   // 右三段式开关
extern float g_adc_hw_version; // 硬件版本

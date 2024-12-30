/**
 * @file serial_led_bsp.h
 * @author WittXie
 * @brief 串行LED驱动 板级支持包
 * @version 0.1
 * @date 2024-10-08
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

// 驱动
#include "./../../lib/serial_led/serial_led.h"

/**
 * @brief BSP驱动初始化
 *
 */
void serial_led_bsp_init(void);

// SERIAL_LED组
extern sled_t *const g_serial_led_group[];
extern const uint32_t SERIAL_LED_GROUP_SIZE;

// SERIAL_LED声明
extern sled_t g_serial_led;

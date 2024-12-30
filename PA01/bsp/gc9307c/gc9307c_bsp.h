/**
 * @file gc9307c_bsp.h
 * @author WittXie
 * @brief LCD屏幕驱动
 * @version 0.1
 * @date 2024-10-15
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
#include "./../../lib/screen/gc9307c/gc9307c.h"

/**
 * @brief BSP驱动初始化
 *
 */
void gc9307c_bsp_init(void);

// GC9307C组
extern gc9307c_t *const g_gc9307c_group[];
extern const uint32_t GC9307C_GROUP_SIZE;

// GC9307C声明
extern gc9307c_t g_gc9307c_lcd;

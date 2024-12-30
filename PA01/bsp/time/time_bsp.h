/**
 * @file time_bsp.h
 * @author WittXie
 * @brief bsp层时间处理工具
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

// 环境
#include "../bsp_env.h"

// 驱动
#include "../../lib/time/time.h"

/**
 * @brief bsp初始化
 *
 */
void time_bsp_init(void);

// time组
extern vtime_t *const g_time_group[];
extern const uint32_t TIME_GROUP_SIZE;

// 声明
extern vtime_t g_time_timer5;

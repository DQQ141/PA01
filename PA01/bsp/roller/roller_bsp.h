/**
 * @file roller_bsp.h
 * @author WittXie
 * @brief 串行LED驱动 板级支持包
 * @version 0.1
 * @date 2024-09-24
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
#include "./../../lib/roller/roller.h"

/**
 * @brief BSP驱动初始化
 *
 */
void roller_bsp_init(void);

// roller组
extern roller_t *const g_roller_group[];
extern const uint32_t ROLLER_GROUP_SIZE;

// roller声明
extern roller_t g_roller;

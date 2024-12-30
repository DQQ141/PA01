/**
 * @file aw9523b_bsp.h
 * @author WittXie
 * @brief IO拓展芯片 AW9523B 驱动 板级支持包
 * @version 0.1
 * @date 2024-09-30
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
#include "./../../lib/exio/aw9523b/aw9523b.h"

/**
 * @brief BSP驱动初始化
 *
 */
void aw9523b_bsp_init(void);

// AW9523B组
extern aw9523b_t *const g_aw9523b_group[];
extern const uint32_t AW9523B_GROUP_SIZE;

// AW9523B声明
extern aw9523b_t g_aw9523b_exio;

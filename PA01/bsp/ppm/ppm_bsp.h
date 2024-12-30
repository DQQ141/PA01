/**
 * @file ppm_bsp.h
 * @author WittXie
 * @brief PPM驱动 板级支持包
 * @version 0.1
 * @date 2024-10-12
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
#include "./../../lib/ppm/ppm.h"

/**
 * @brief BSP驱动初始化
 *
 */
void ppm_bsp_init(void);

// PPM组
extern ppm_t *const g_ppm_group[];
extern const uint32_t PPM_GROUP_SIZE;

// PPM声明
extern ppm_t g_ppm_main;

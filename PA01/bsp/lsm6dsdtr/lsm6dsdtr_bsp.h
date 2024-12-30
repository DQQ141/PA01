/**
 * @file lsm6dsdtr_bsp.h
 * @author WittXie
 * @brief LSM6DSDTR陀螺仪芯片驱动
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
#include "./../../lib/imu/lsm6dsdtr/lsm6dsdtr.h"

/**
 * @brief BSP驱动初始化
 *
 */
void lsm6dsdtr_bsp_init(void);

// LSM6DSDTR组
extern lsm6dsdtr_t *const g_lsm6dsdtr_group[];
extern const uint32_t LSM6DSDTR_GROUP_SIZE;

// LSM6DSDTR声明
extern lsm6dsdtr_t g_lsm6dsdtr_imu;

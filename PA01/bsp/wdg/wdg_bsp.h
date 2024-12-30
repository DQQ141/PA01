/**
 * @file wdg_bsp.h
 * @author WittXie
 * @brief 看门狗
 * @version 0.1
 * @date 2024-12-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

// 环境
#include "../bsp_env.h"

// 依赖
#include "./../gpio/gpio_bsp.h"
#include "./../log/log_bsp.h"
#include "./../os/os_bsp.h"
#include "./../time/time_bsp.h"

/**
 * @brief 初始化看门狗
 *
 */
void wdg_bsp_init(void);

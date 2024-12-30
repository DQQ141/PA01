/**
 * @file lvgl_bsp.h
 * @author WittXie
 * @brief LVGL驱动
 * @version 0.1
 * @date 2024-10-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

// 环境
#include "../bsp_env.h"

// 依赖
#include "./../gc9307c/gc9307c_bsp.h"
#include "./../gpio/gpio_bsp.h"
#include "./../log/log_bsp.h"
#include "./../os/os_bsp.h"
#include "./../roller/roller_bsp.h"
#include "./../time/time_bsp.h"

extern void *lvgl_mutex;

#define lvgl_lock()                                    \
    {                                                  \
        if (os_mutex_lock(&lvgl_mutex, 1000) == false) \
        {                                              \
            log_error("lvgl lock failed");             \
        }                                              \
    }

#define lvgl_unlock()                 \
    {                                 \
        os_mutex_unlock(&lvgl_mutex); \
    }

/**
 * @brief BSP驱动初始化
 *
 */
void lvgl_bsp_init(void);

/**
 * @brief LVGL 驱动刷新
 *
 */
void lvgl_bsp_flush(void);

/**
 * @brief LVGL 是否在运行
 *
 * @return true
 * @return false
 */
bool lvgl_is_running(void);

#include "./lvgl/lvgl.h"

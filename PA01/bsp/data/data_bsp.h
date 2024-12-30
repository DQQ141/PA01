/**
 * @file data_bsp.h
 * @author WittXie
 * @brief data驱动
 * @version 0.1
 * @date 2024-11-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

// 环境
#include "../bsp_env.h"

// 依赖
#include "./../crc/crc_bsp.h"
#include "./../log/log_bsp.h"
#include "./../os/os_bsp.h"
#include "./../time/time_bsp.h"

// 驱动
#include "./../../lib/data/data.h"

// 设备列表
extern data_t g_data_factory;

// 设备组
extern data_t *const g_data_group[];
extern const uint32_t DATA_GROUP_SIZE;

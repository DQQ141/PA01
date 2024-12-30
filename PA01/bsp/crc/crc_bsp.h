/**
 * @file crc.h
 * @author WittXie
 * @brief CRC校验驱动
 * @version 0.1
 * @date 2024-10-17
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
#include "./../../lib/encryptor/crc/crc.h"

/**
 * @brief BSP驱动初始化
 *
 */
void crc_bsp_init(void);

// CRC组
extern crc_t *const g_crc_group[];
extern const uint32_t CRC_GROUP_SIZE;

// CRC声明
extern crc_t g_crc_ccitt; // CCITT

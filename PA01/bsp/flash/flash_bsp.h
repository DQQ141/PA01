/**
 * @file flash_bsp.h
 * @author WittXie
 * @brief FLASH板级支持包
 * @version 0.1
 * @date 2024-09-22
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../bsp_env.h"

// 依赖
#include "./../log/log_bsp.h"
#include "./../time/time_bsp.h"

// 驱动
#include "../../lib/flash/flash.h"

/**
 * @brief bsp初始化
 *
 */
void flash_bsp_init(void);

// GPIO组
extern flash_t *const g_flash_group[];
extern const uint32_t FLASH_GROUP_SIZE;

// GPIO声明
extern flash_t g_flash_mx25;

/**
 * @brief 映射模式
 *
 */
void flash_remap_mode(void);

/**
 * @brief 手动模式
 *
 */
void flash_manual_mode(void);
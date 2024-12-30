/**
 * @file protocol.h
 * @author WittXie
 * @brief 霍尔摇杆驱动
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
#include "./../../lib/protocol/protocol.h"

/**
 * @brief BSP驱动初始化
 *
 */
void protocol_bsp_init(void);

// PROTOCOL组
extern protocol_t *const g_protocol_group[];
extern const uint32_t PROTOCOL_GROUP_SIZE;

// PROTOCOL声明
extern protocol_t g_protocol_uart_stick;
extern protocol_t g_protocol_uart_rf;
extern protocol_t g_protocol_uart_head;
extern protocol_t g_protocol_uart_sport;

/**
 * @brief 设置RF模式
 * @param is_update 是否更新模式
 *
 */
void rf_update_mode_set(bool is_update);
bool rf_update_mode_get(void);

/**
 * @brief RF重启
 *
 */
void rf_reboot(void);

// 协议
#include "./../../lib/protocol/fs_inrm303/fs_inrm303.h"
#include "./../../lib/protocol/fs_stick/fs_stick.h"

// 协议声明
extern fs_stick_t g_fs_stick;
extern fs_inrm303_t g_fs_inrm303;

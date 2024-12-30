/**
 * @file player_bsp.h
 * @author WittXie
 * @brief 播放驱动
 * @version 0.1
 * @date 2024-09-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

// 环境
#include "../bsp_env.h"

// 依赖
#include "./../../lib/player/player.h"
#include "./../log/log_bsp.h"
#include "./../os/os_bsp.h"
#include "./../time/time_bsp.h"

/**
 * @brief bsp初始化
 *
 */
void player_bsp_init(void);

// 设备组
extern player_t *g_player_group[];
extern const uint32_t PLAYER_GROUP_SIZE;

// 设备声明
extern player_t g_player;

// 资源
extern voice_t g_voice_dada;
extern voice_t g_voice_entry;
extern voice_t g_voice_error;
extern voice_t g_voice_exit;
extern voice_t g_voice_msg;
extern voice_t g_voice_select;
extern voice_t g_voice_welcome;

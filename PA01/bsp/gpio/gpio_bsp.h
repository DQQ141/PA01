/**
 * @file gpio_bsp.h
 * @author WittXie
 * @brief 板级支持包GPIO接口
 * @version 0.1
 * @date 2024-05-09
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
#include "../../lib/gpio/gpio.h"

/**
 * @brief bsp初始化
 *
 */
void gpio_bsp_init(void);

// 设备组
extern gpio_t *const g_gpio_group[];
extern const uint32_t GPIO_GROUP_SIZE;

// 设备声明
extern gpio_t g_rgb_left1;
extern gpio_t g_rgb_left2;
extern gpio_t g_rgb_power1;
extern gpio_t g_rgb_power2;
extern gpio_t g_rgb_11;
extern gpio_t g_rgb_12;
extern gpio_t g_rgb_21;
extern gpio_t g_rgb_22;
extern gpio_t g_rgb_31;
extern gpio_t g_rgb_32;
extern gpio_t g_rgb_41;
extern gpio_t g_rgb_42;
extern gpio_t g_rgb_right2;
extern gpio_t g_rgb_right1;
extern gpio_t g_rgb_power;
extern gpio_t g_rgb_power;
extern gpio_t g_btn_slw_left;
extern gpio_t g_btn_roller;
extern gpio_t g_btn_right;
extern gpio_t g_btn_power;
extern gpio_t g_btn_dfu;
extern gpio_t g_btn_back_left;
extern gpio_t g_btn_back_right;
extern gpio_t g_exbtn_tr1n;
extern gpio_t g_exbtn_tr1p;
extern gpio_t g_exbtn_tr2n;
extern gpio_t g_exbtn_tr2p;
extern gpio_t g_exbtn_tr3n;
extern gpio_t g_exbtn_tr3p;
extern gpio_t g_exbtn_tr4p;
extern gpio_t g_exbtn_tr4n;
extern gpio_t g_exbtn_pp;
extern gpio_t g_exbtn_pn;
extern gpio_t g_exbtn_rtn;
extern gpio_t g_exbtn_menu;
extern gpio_t g_exbtn_button1;
extern gpio_t g_exbtn_button2;
extern gpio_t g_exbtn_button3;
extern gpio_t g_exbtn_button4;
extern gpio_t g_out_power;
extern gpio_t g_exout_charge_enable;
extern gpio_t g_exout_gpio0;
extern gpio_t g_exout_u6_select_exrf;
extern gpio_t g_exout_rfpower_enable;
extern gpio_t g_exout_exrfpower_enable;
extern gpio_t g_pwm_vibration;
extern gpio_t g_pwm_lcd_light;

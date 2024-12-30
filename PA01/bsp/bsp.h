/**
 * @file bsp.h
 * @author WittXie
 * @brief 板级支持包汇总头文件
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

// env
#include "./bsp_env.h"

// bsp
#include "./adc/adc_bsp.h"
#include "./aw9523b/aw9523b_bsp.h"
#include "./crc/crc_bsp.h"
#include "./data/data_bsp.h"
#include "./flash/flash_bsp.h"
#include "./gc9307c/gc9307c_bsp.h"
#include "./gpio/gpio_bsp.h"
#include "./log/log_bsp.h"
#include "./lsm6dsdtr/lsm6dsdtr_bsp.h"
#include "./lvgl/lvgl_bsp.h"
#include "./os/os_bsp.h"
#include "./player/player_bsp.h"
#include "./ppm/ppm_bsp.h"
#include "./protocol/protocol_bsp.h"
#include "./roller/roller_bsp.h"
#include "./serial_led/serial_led_bsp.h"
#include "./time/time_bsp.h"
#include "./wdg/wdg_bsp.h"
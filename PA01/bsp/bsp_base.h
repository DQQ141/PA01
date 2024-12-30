/**
 * @file bsp_base.h
 * @author WittXie
 * @brief 板级支持包基础函数头文件
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

// env
#include "./bsp_env.h"

// 标准库
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 基础控件
#include "./../lib/algorithm/compare/compare.h"
#include "./../lib/algorithm/fit/fit.h"
#include "./../lib/algorithm/sort/sort.h"
#include "./../lib/dds/dds.h"
#include "./../lib/list/list.h"
#include "./../lib/ring/ring.h"
#include "./../lib/string/string.h"

// 滤波器
#include "./../lib/filter/lp_filter.h" // 低通滤波器

// 圆周率
#ifndef M_PI
#define M_PI 3.14159265359f
#endif

/**
 * @brief 重启
 *
 */
void bsp_reboot(void);

/**
 * @brief 打印是否调试
 *
 */
void bsp_print_debug_mode(void);

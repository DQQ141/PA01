/**
 * @file rf_update.cc
 * @author WittXie
 * @brief RF模块烧录
 * @version 0.1
 * @date 2024-11-25
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../factory_app.h"

// #undef log_trace
// #define log_trace(_format, ...)

// RF模块更新
static void rf_update_init(void)
{
    gpio_write(&g_exout_u6_select_exrf, true); //  启用EXRF接口

    // rf_update_mode_set(true); // RF模块更新
    rf_update_mode_set(false);
}

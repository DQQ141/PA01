/**
 * @file power.c
 * @author WittXie
 * @brief 电源相关
 * @version 0.1
 * @date 2024-11-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../factory_app.h"

// 按键响应
#include "./src/power_battery.cc"

void factory_power_init(void)
{
    os_rely_wait(adc_is_running(), 6000); // 依赖 adc

    power_battery_init(); // 电池供电
}

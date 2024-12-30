/**
 * @file button.c
 * @author WittXie
 * @brief 响应按键
 * @version 0.1
 * @date 2024-11-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../factory_app.h"

// 按键响应
#include "./src/button_effect.cc"
#include "./src/button_lcd_light.cc"
#include "./src/button_power.cc"
#include "./src/button_reboot.cc"

void factory_button_init(void)
{
    for (int i = 0; i < GPIO_GROUP_SIZE; i++)
    {
        os_rely_wait(gpio_is_running(g_gpio_group[i]), 6000); // 依赖 gpio
    }
    os_rely_wait(roller_is_running(&g_roller), 6000); // 依赖旋钮

    button_power_init(); // 电源按键
    // button_reboot_init();    // 重启按键
    button_effect_init();    // 音效
    button_lcd_light_init(); // 背光按键
}

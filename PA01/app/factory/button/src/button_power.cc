/**
 * @file power.cc
 * @author WittXie
 * @brief 电源按钮
 * @version 0.1
 * @date 2024-10-28
 * @note 电源键比较特殊，需要特别处理
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../factory_app.h"

static void button_power_long_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    // 按下已经超过2秒
    if (g_factory.status.is_running == true)
    {
        // 关闭所有外设
        gc9307c_clear(&g_gc9307c_lcd, GC9307C_COLOR_BLACK);
        gc9307c_enter_sleep(&g_gc9307c_lcd);
        gpio_write(&g_pwm_lcd_light, false);
        for (uint32_t i = 0; i < g_serial_led.cfg.led_num; i++)
        {
            serial_led_hsv_set(&g_serial_led, i, 0, 0, 0); // 关闭LED
        }
        gpio_write(&g_rgb_power, false); // 关闭电源RGB
        log_info("system shutdown now...");

        os_critical_enter();                    // 进入临界区
        while (gpio_read(&g_btn_power) == true) // 等待松手
        {
        }
        bsp_reboot(); // 重启
    }
    else
    {
        gpio_write(&g_out_power, true); // 打开电源
        g_factory.status.is_running = true;
        log_info(COLOR_H_GREEN "power on");
    }
}

static void button_power_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    if (topic == &g_btn_power.TO_VALID)
    {
        // 按下
        gpio_value_write(&g_rgb_power, (sled_color_t){.rgb = {.r = 64, .g = 32, .b = 0}}.value); // 打开电源RGB
    }
    else if (topic == &g_btn_power.TO_INVALID)
    {
        // 松开
        gpio_write(&g_rgb_power, false); // 关闭电源RGB
    }
}
static void button_power_init(void)
{
    g_btn_power.cfg.timegap_long_press = 2000000lu; // 长按触发设置为3秒响应

    // 开启电源RGB灯
    gpio_value_write(&g_rgb_power, (sled_color_t){.rgb = {.r = 0, .g = 64, .b = 0}}.value); // 打开电源RGB
    os_critical_enter();

    // 电源按钮被按下2秒时，打开电源
    while (gpio_read(&g_btn_power) == true)
    {
        if (TIMESTAMP_US - TIMESTAMP_US_STARTUP > g_btn_power.cfg.timegap_long_press)
        {
            gpio_write(&g_out_power, true); // 打开电源
            g_factory.status.is_running = true;
            log_info(COLOR_H_GREEN "power on");
            break;
        }
    }
    gpio_write(&g_rgb_power, false); // 关闭电源RGB灯
    os_critical_exit();

    // 电源灯控制
    dds_subcribe(&g_btn_power.TO_VALID, DDS_PRIORITY_NORMAL, &button_power_callback, NULL);
    dds_subcribe(&g_btn_power.TO_INVALID, DDS_PRIORITY_NORMAL, &button_power_callback, NULL);

    // 开关机
    dds_subcribe(&g_btn_power.TO_KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, &button_power_long_callback, NULL);
}

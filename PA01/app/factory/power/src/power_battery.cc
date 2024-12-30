/**
 * @file battery.cc
 * @author WittXie
 * @brief 电池相关
 * @version 0.1
 * @date 2024-10-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../../factory_app.h"

static void power_battery_entry(void *args)
{
    gpio_write(&g_exout_charge_enable, false);
    float last_adc_usb = 0;
    for (;;)
    {
        os_sleep(200);
        if (g_adc_usb > 0.3f && last_adc_usb < 0.3f)
        {
            g_factory.status.is_charging = true;
            gpio_write(&g_exout_charge_enable, true);
            // 开启电源RGB灯
            gpio_value_write(&g_rgb_power, (sled_color_t){.rgb = {.r = 0, .g = 0, .b = 255}}.value);
            log_info("charge enable.");
        }
        if (g_adc_usb < 0.3f && last_adc_usb > 0.3f)
        {
            g_factory.status.is_charging = false;
            gpio_write(&g_exout_charge_enable, false);
            gpio_write(&g_rgb_power, false);
            log_info("charge disable.");
        }
        last_adc_usb = g_adc_usb;

        // 正在充电
        // 0.636~0.909 <=> 2.1V~3V <=> 0~100%
        if (gpio_read(&g_exout_charge_enable) == true)
        {
            gpio_write(&g_exout_charge_enable, false);
            os_sleep(5);
            g_factory.value.battery = (g_adc_battery - 0.636f) / (0.909f - 0.636f); // 更新电池电压
            gpio_write(&g_exout_charge_enable, true);
        }
        else
        {
            g_factory.value.battery = (g_adc_battery - 0.636f) / (0.909f - 0.636f);
            if (g_factory.value.battery < 0)
            {
                // 关机
                log_error("battery[%3.02f%%] low, system shutdown after 3s.", g_factory.value.battery * 100.0f);
                os_sleep(3000);
                bsp_reboot();
            }
        }
    }
}

static void power_battery_init(void)
{
    os_task_create(power_battery_entry, "power_battery", NULL, (OS_PRIORITY_APP + OS_PRIORITY_BSP) / 2, OS_TASK_STACK_MIN + 1024);
}

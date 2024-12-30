/**
 * @file button_lcd_light.cc
 * @author WittXie
 * @brief 背光控制
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../factory_app.h"

static void button_lcd_light_sub_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    if (gpio_value_get(&g_pwm_lcd_light) + 20 <= 100)
    {
        gpio_value_set(&g_pwm_lcd_light, gpio_value_get(&g_pwm_lcd_light) + 20);
    }
    gpio_value_write(&g_pwm_lcd_light, g_pwm_lcd_light.cfg.value * 100);
}

static void button_lcd_light_unsub_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    if (gpio_value_get(&g_pwm_lcd_light) - 20 > 0)
    {
        gpio_value_set(&g_pwm_lcd_light, gpio_value_get(&g_pwm_lcd_light) - 20);
    }
    gpio_value_write(&g_pwm_lcd_light, g_pwm_lcd_light.cfg.value * 100);
}

static void button_lcd_light_idle(void *userdata)
{
    if (CMP_PREV_FLOAT(g_factory.value.lcd_light, 0.001f) != 0)
    {
        gpio_value_write(&g_pwm_lcd_light, g_factory.value.lcd_light * 100);
    }
}

static void button_lcd_light_init(void)
{
    // dds_subcribe(&g_exbtn_tr2p.PRESSED, DDS_PRIORITY_NORMAL, &button_lcd_light_unsub_callback, NULL);
    // dds_subcribe(&g_exbtn_tr2p.KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, &button_lcd_light_unsub_callback, NULL);
    // dds_subcribe(&g_exbtn_tr2n.PRESSED, DDS_PRIORITY_NORMAL, &button_lcd_light_sub_callback, NULL);
    // dds_subcribe(&g_exbtn_tr2n.KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, &button_lcd_light_sub_callback, NULL);

    dds_idle_create(&button_lcd_light_idle, NULL, DDS_PRIORITY_NORMAL, 100000);
}

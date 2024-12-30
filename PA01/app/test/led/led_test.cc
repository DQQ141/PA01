/**
 * @file led_test.cc
 * @author WittXie
 * @brief LED测试
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static uint16_t s_hue = 180;
static uint8_t s_bright = 32;

static void led_entry(void *args)
{
    for (;;)
    {
        os_sleep(5);
        for (uint32_t i = 0; i < g_serial_led.cfg.led_num; i++)
        {
            if (i == 8 || i == 9) // 跳过电源LED
                continue;

            if (gpio_read(&g_btn_slw_left) == true)
            {
                serial_led_hsv_set(&g_serial_led, i, (s_hue + i * 60) % 360, 255, s_bright); // 异色
            }
            else
            {
                serial_led_hsv_set(&g_serial_led, i, (s_hue) % 360, 255, s_bright); // 同色
            }
        }
        s_hue++;
    }
}

static void led_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    if (device == &g_exbtn_tr3n || device == &g_exbtn_tr3p)
    {
        // 调节颜色，删除颜色自动变化
        TaskHandle_t task_handle = os_task_find("led_test");
        if (task_handle != NULL)
        {
            os_task_delete(task_handle);
        }
    }

    if (device == &g_exbtn_tr1n) // 亮度 +
    {
        if (s_bright + 16 < 255)
        {
            s_bright += 16;
        }
        else
        {
            s_bright = 255;
        }
    }
    else if (device == &g_exbtn_tr1p) // 亮度 -
    {
        if (s_bright - 16 > 0)
        {
            s_bright -= 16;
        }
        else
        {
            s_bright = 0;
        }
    }
    else if (device == &g_exbtn_tr3n) // 颜色 +
    {
        s_hue = (s_hue + 10) % 360;
    }
    else if (device == &g_exbtn_tr3p) // 颜色 -
    {
        s_hue = (s_hue - 10) % 360;
    }

    for (uint32_t i = 0; i < g_serial_led.cfg.led_num; i++)
    {
        if (i == 8 || i == 9) // 跳过电源LED
            continue;

        if (gpio_read(&g_btn_slw_left) == true)
        {
            serial_led_hsv_set(&g_serial_led, i, (s_hue + i * 60) % 360, 255, s_bright); // 异色
        }
        else
        {
            serial_led_hsv_set(&g_serial_led, i, (s_hue) % 360, 255, s_bright); // 同色
        }
    }
    os_sleep(100);
}

static void led_test(void)
{
    os_task_create(led_entry, "led_test", NULL, OS_PRIORITY_APP, OS_TASK_STACK_MIN + 512);

    // 变亮度
    dds_subcribe(&g_exbtn_tr1n.PRESSED, DDS_PRIORITY_NORMAL, led_callback, NULL);
    dds_subcribe(&g_exbtn_tr1p.PRESSED, DDS_PRIORITY_NORMAL, led_callback, NULL);
    dds_subcribe(&g_exbtn_tr1n.KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, led_callback, NULL);
    dds_subcribe(&g_exbtn_tr1p.KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, led_callback, NULL);

    // 变颜色
    dds_subcribe(&g_exbtn_tr3n.PRESSED, DDS_PRIORITY_NORMAL, led_callback, NULL);
    dds_subcribe(&g_exbtn_tr3p.PRESSED, DDS_PRIORITY_NORMAL, led_callback, NULL);
    dds_subcribe(&g_exbtn_tr3n.KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, led_callback, NULL);
    dds_subcribe(&g_exbtn_tr3p.KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, led_callback, NULL);
}

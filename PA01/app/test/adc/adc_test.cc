/**
 * @file adc_test.cc
 * @author WittXie
 * @brief ADC测试
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void adc_entry(void *args)
{
    for (;;)
    {
        os_sleep(100);
        if (gpio_read(&g_btn_slw_left) == true)
        {
            if (g_adc_sw_right > 0.75)
            {
                print(COLOR_H_GREEN "\rusb = %4.02f, battery = %4.02f, pot_left = %4.02f, pot_right = %4.02f, sw_left = %4.02f, sw_right = %4.02f" COLOR_H_WHITE ASCII_CLEAR_TAIL,
                      g_adc_usb, g_factory.value.battery, g_adc_pot_left, g_adc_pot_right, g_adc_sw_left, g_adc_sw_right);
            }

            if (CMP_PREV_FLOAT(g_adc_usb, 0.1f) != 0)
            {
                log_info("usb = %4.02f", g_adc_usb);
            }
            if (CMP_PREV_FLOAT(g_adc_pot_left, 0.1f) != 0)
            {
                log_info("pot_left = %4.02f", g_adc_pot_left);
            }
            if (CMP_PREV_FLOAT(g_adc_pot_right, 0.1f) != 0)
            {
                log_info("pot_right = %4.02f", g_adc_pot_right);
            }
            if (CMP_PREV_FLOAT(g_adc_sw_left, 0.1f) != 0)
            {
                log_info("sw_left = %4.02f", g_adc_sw_left);
            }
            if (CMP_PREV_FLOAT(g_adc_sw_right, 0.1f) != 0)
            {
                log_info("sw_right = %4.02f", g_adc_sw_right);
            }
        }
    }
}
static void adc_test(void)
{
    os_task_create(adc_entry, "adc_test", NULL, OS_PRIORITY_APP, OS_TASK_STACK_MIN + 2048);
}

/**
 * @file lsm6dsdtr_test.cc
 * @author WittXie
 * @brief 测试陀螺仪
 * @version 0.1
 * @date 2024-09-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void lsm6dsdtr_entry(void *args)
{
    for (;;)
    {
        os_sleep(100);
        if (gpio_read(&g_btn_slw_left) == true && g_adc_sw_right < 0.25)
        {
            print(COLOR_H_CYAN "\rx = %6.02f, y = %6.02f, z = %6.02f" COLOR_H_WHITE ASCII_CLEAR_TAIL,
                  g_lsm6dsdtr_imu.data.angle.x, g_lsm6dsdtr_imu.data.angle.y, g_lsm6dsdtr_imu.data.angle.z);
        }
    }
}

static void lsm6dsdtr_test(void)
{
    os_task_create(lsm6dsdtr_entry, "lsm6dsdtr_test", NULL, OS_PRIORITY_APP, OS_TASK_STACK_MIN + 1024);
}

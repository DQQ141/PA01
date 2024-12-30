/**
 * @file stick_test.cc
 * @author WittXie
 * @brief HALL摇杆测试
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void stick_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    if (gpio_read(&g_btn_slw_left) == true)
    {
        fs_stick_t *stick = (fs_stick_t *)device;
        print("\rstick[%s], CH1[%5.02f, %5.02f], CH2[%5.02f,%5.02f]" ASCII_CLEAR_TAIL "\r\n",
              stick->cfg.name, stick->stick[0].x, stick->stick[0].y, stick->stick[1].x, stick->stick[1].y);
    }
}
static void stick_test(void)
{
    dds_subcribe(&g_fs_stick.KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, stick_callback, NULL);
}

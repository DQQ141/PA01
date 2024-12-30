/**
 * @file button_reboot.cc
 * @author WittXie
 * @brief 按键重启
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../../factory_app.h"

static void button_reboot_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    log_info("button reboot...");
    os_sleep(100);

    // 重启
    bsp_reboot();
}

static void button_reboot_init(void)
{
    dds_subcribe(&g_exbtn_button2.PRESSED, DDS_PRIORITY_LOW, &button_reboot_callback, NULL);
    // dds_subcribe(&g_exbtn_button2.KEEP_LONG_PRESS, DDS_PRIORITY_LOW, &button_reboot_callback, NULL);
}

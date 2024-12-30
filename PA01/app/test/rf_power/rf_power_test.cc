/**
 * @file rf_power_test.cc
 * @author WittXie
 * @brief RF电源测试
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void rf_power_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    bool enable = gpio_read(&g_btn_slw_left);
    gpio_write(&g_exout_rfpower_enable, enable);
    gpio_write(&g_exout_exrfpower_enable, enable);
    log_info("RF Power %s.", gpio_read(&g_exout_rfpower_enable) ? "Enable" : "Disable");
    log_info("EXRF Power %s.", gpio_read(&g_exout_exrfpower_enable) ? "Enable" : "Disable");
}

static void rf_power_test(void)
{
    gpio_write(&g_exout_rfpower_enable, true);
    gpio_write(&g_exout_exrfpower_enable, true);
    log_info("RF Power %s.", gpio_read(&g_exout_rfpower_enable) ? "Enable" : "Disable");
    log_info("EXRF Power %s.", gpio_read(&g_exout_exrfpower_enable) ? "Enable" : "Disable");

    log_info("RF Power Mode %s.", aw9523b_pin_config_get(&g_aw9523b_exio, 7) ? "Enable" : "Disable");
    log_info("EXRF Power Mode %s.", aw9523b_pin_config_get(&g_aw9523b_exio, 15) ? "Enable" : "Disable");

    dds_subcribe(&g_btn_slw_left.CHANGED, DDS_PRIORITY_NORMAL, &rf_power_callback, NULL);
}

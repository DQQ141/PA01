/**
 * @file ppm_test.cc
 * @author WittXie
 * @brief 测试PPM
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void ppm_button_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    static uint8_t cnt = 0;
    static bool valid = true;

    log_info("%s write channel_%d value to %s", g_ppm_main.cfg.name, cnt, valid ? "true" : "false");
    ppm_channel_write(&g_ppm_main, cnt, valid);

    cnt++;
    cnt = cnt % g_ppm_main.cfg.channels;

    if (cnt == 0)
    {
        valid = !valid;
    }
}
static void ppm_update_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    log_info("%s RECEIVED_DATA_CHANGED.", g_ppm_main.cfg.name);

    for (uint32_t i = 0; i < g_ppm_main.cfg.channels; i++)
    {
        print("channel_%d: %d\r\n", i, ppm_receive_channel_read(&g_ppm_main, i));
    }
}

static void ppm_test(void)
{
    dds_subcribe(&g_exbtn_button1.PRESSED, DDS_PRIORITY_NORMAL, &ppm_button_callback, NULL);
    dds_subcribe(&g_ppm_main.RECEIVED_DATA_CHANGED, DDS_PRIORITY_NORMAL, &ppm_update_callback, NULL);
}

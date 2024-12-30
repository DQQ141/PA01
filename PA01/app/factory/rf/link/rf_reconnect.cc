/**
 * @file rf_reconnect.cc
 * @author WittXie
 * @brief RF重连
 * @version 0.1
 * @date 2024-12-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../factory_app.h"

static struct
{
    uint8_t count;
} s_rf_reconnect = {
    .count = 0,
};

static void rf_reconnect_receive_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    protocol_t *protocol = (protocol_t *)device;
    protocol_frame_t *frame = (protocol_frame_t *)arg;
    s_rf_reconnect.count = 0;
}

static void rf_reconnect_idle(void *userdata)
{
    s_rf_reconnect.count++;
    if (s_rf_reconnect.count > 20)
    {
        s_rf_reconnect.count = 0;
        if (rf_update_mode_get() == false)
        {
            rf_reboot();
        }
    }
}

// 调试初始化
static void rf_reconnect_init(void)
{
    dds_subcribe(&g_protocol_uart_rf.RECEIVE, DDS_PRIORITY_NORMAL, rf_reconnect_receive_callback, NULL);
    dds_idle_create(rf_reconnect_idle, NULL, DDS_PRIORITY_NORMAL, 100000);
}

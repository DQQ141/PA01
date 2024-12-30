/**
 * @file roller_test.cc
 * @author WittXie
 * @brief 滚轮测试
 * @version 0.1
 * @date 2024-09-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void roller_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    roller_t *roller = (roller_t *)device;

    // 打印roller
    print("%s: step = %ld.\r\n", roller->cfg.name, roller->step);
}

static void roller_test(void)
{
    dds_subcribe(&g_roller.CHANGED, DDS_PRIORITY_NORMAL, &roller_callback, NULL);
}

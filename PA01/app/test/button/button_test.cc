/**
 * @file button_test.cc
 * @author WittXie
 * @brief 测试按键
 * @version 0.1
 * @date 2024-09-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void button_print_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    gpio_t *gpio = (gpio_t *)device;
    log_info("%s pressed.", gpio->cfg.name);
}

static void button_test(void)
{
    for (int i = 0; i < GPIO_GROUP_SIZE; i++)
    {
        if (strstr(g_gpio_group[i]->cfg.name, "btn_") != NULL)
        {
            dds_subcribe(&g_gpio_group[i]->PRESSED, DDS_PRIORITY_NORMAL, &button_print_callback, NULL);
        }
    }
}

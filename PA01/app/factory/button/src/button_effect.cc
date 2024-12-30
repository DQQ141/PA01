/**
 * @file button_effect.cc
 * @author WittXie
 * @brief 按键反馈效果
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../../factory_app.h"

void ui_hal_play(voice_t *voice, uint16_t cnt);

static void button_effect_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    // 打印按键信息
    gpio_t *gpio = (gpio_t *)device;
    log_info("%s pressed.", gpio->cfg.name);

    // 播放音频
    ui_hal_play(&g_voice_select, 1);
}

static void roller_effect_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    int32_t step = *(int32_t *)arg;
    step = step < 0 ? -step : step;
    step = step > 8 ? 8 : step;

    // 播放音频
    ui_hal_play(&g_voice_select, step);
}

static void button_effect_init(void)
{
    // 设置默认参数
    g_factory.value.volume = 1.0f;    // 默认音量
    g_factory.value.vibration = 0.5f; // 默认震动 50%

    // 订阅按键消息
    // for (int i = 0; i < GPIO_GROUP_SIZE; i++)
    // {
    //     // 名称包含btn
    //     if (strstr(g_gpio_group[i]->cfg.name, "btn_") != NULL)
    //     {
    //         dds_subcribe(&g_gpio_group[i]->PRESSED, DDS_PRIORITY_NORMAL, &button_effect_callback, NULL);
    //     }
    // }
    dds_subcribe(&g_btn_slw_left.TO_KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, &button_effect_callback, NULL);
    dds_subcribe(&g_btn_slw_left.TO_KEEP_LONG_RELEASE, DDS_PRIORITY_NORMAL, &button_effect_callback, NULL);

    dds_subcribe(&g_roller.CHANGED, DDS_PRIORITY_NORMAL, &roller_effect_callback, NULL);
}

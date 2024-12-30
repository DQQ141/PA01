/**
 * @file voice.cc
 * @author WittXie
 * @brief 声音播放器端口
 * @version 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../player_bsp.h"

#undef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_NONE

#define GPIO_VOICE_ENABLE GPIOE, GPIO_PIN_4

extern DMA_HandleTypeDef hdma_dac1_ch1;

// 声音播放器
static void voice_init(voice_t *voice)
{
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);
    HAL_TIM_Base_Start(&htim15); // 打开定时器
}

static uint32_t voice_play(void *data, uint32_t length)
{
    uint32_t send_length = 0;
    static uint32_t remain = 0;
    uint32_t current_remain = 0;

    // 如果还有数据未发送
    if (length > 0)
    {
        current_remain = __HAL_DMA_GET_COUNTER(&hdma_dac1_ch1);
        if ((HAL_DMA_GetState(&hdma_dac1_ch1) == HAL_DMA_STATE_READY) && (remain == current_remain)) // 等待DMA之前的数据发完
        {
            {
                // 计算本次可以发送的最大数据长度
                send_length = (length > UINT16_MAX) ? UINT16_MAX : length;

                log_trace("send_length: %d, length: %d, hdma_is_ready:%s, hdma_counter:%d",
                          send_length, length, HAL_DMA_GetState(&hdma_dac1_ch1) == HAL_DMA_STATE_READY ? "ready" : "not", current_remain);

                // 发送数据
                HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, data, send_length, DAC_ALIGN_12B_R); // 继续DMA-DAC传输
            }
        }
        remain = current_remain;
    }

    // 返回本次发送的数据长度
    return send_length;
}

static bool voice_is_ready(voice_t *voice)
{
    return (HAL_DAC_GetState(&hdac1) == HAL_DAC_STATE_READY);
}

static void voice_stop(voice_t *voice)
{
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);
    HAL_GPIO_WritePin(GPIO_VOICE_ENABLE, GPIO_PIN_RESET);
}

static void voice_enable(voice_t *voice)
{
    HAL_GPIO_WritePin(GPIO_VOICE_ENABLE, GPIO_PIN_SET); // PA8 ENABLE
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);
    os_sleep(120); // 等待PA8稳定
}

player_t g_player = {
    .cfg = {
        .name = "g_player",
        .data_bits = 12,
    },
    .ops = {
        .init = voice_init,
        .play = voice_play,
        .stop = voice_stop,
        .pause = voice_stop,
        .enable = voice_enable,
        .is_ready = voice_is_ready,
    },
};

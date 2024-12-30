/**
 * @file pwm.cc
 * @author WittXie
 * @brief pwm驱动
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../ppm_bsp.h"

static void ppm_main_init(void)
{
}
static void ppm_main_deinit(void)
{
    HAL_TIM_PWM_DeInit(&htim1);                  // 停止定时器
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_2); // 停止DMA
}
static void ppm_main_set_timer(uint32_t timer_prescaler, uint32_t timer_period)
{
    uint8_t ret = 0;
    // 等待DMA传输完成
    extern DMA_HandleTypeDef hdma_tim1_ch2;
    while (HAL_DMA_GetState(&hdma_tim1_ch2) != HAL_DMA_STATE_READY)
    {
        os_sleep(1);
    }

    // 设置定时器周期值
    ret += HAL_TIM_PWM_DeInit(&htim1);      // 停止PWM
    htim1.Init.Prescaler = timer_prescaler; // 定时器时钟分频
    htim1.Init.Period = timer_period;       // 定时器周期
    ret += HAL_TIM_PWM_Init(&htim1);        // 启动PWM

    if (ret != HAL_OK)
    {
        extern ppm_t g_ppm_main;
        log_error("%s set timer error.", g_ppm_main.cfg.name);
    }
}
static void ppm_main_start(void *pulse, uint32_t pulse_length)
{
    uint8_t ret = 0;
    // 等待DMA传输完成
    extern DMA_HandleTypeDef hdma_tim1_ch2;
    while (HAL_DMA_GetState(&hdma_tim1_ch2) != HAL_DMA_STATE_READY)
    {
        os_sleep(1);
    }

    // 重置定时器
    ret += HAL_TIM_PWM_DeInit(&htim1); // 停止PWM
    ret += HAL_TIM_PWM_Init(&htim1);   // 启动PWM

    // 启动DMA传输
    ret += HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)pulse, pulse_length); // 启动DMA

    if (ret != HAL_OK)
    {
        extern ppm_t g_ppm_main;
        ERROR("%s start error\r\n", g_ppm_main.cfg.name);
    }
}
ppm_t g_ppm_main = {
    .cfg = {
        .name = "g_ppm_main",
        .timer_bits = 16,           // 16位定时器
        .timer_clk = 240 * 1000000, // 定时器时钟 = 240MHz
        .period_us = 200,           // 最小脉冲周期值, 单位us
        .channels = 8,              // 通道
        .n_t0 = 1,                  // T0L时间n个最小脉冲周期
        .n_t1 = 2,                  // T1L时间n个最小脉冲周期
        .tolerance_us = 20,         // 最大允许误差, 单位us
    },
    .ops = {
        .init = ppm_main_init,
        .deinit = ppm_main_deinit,
        .set_timer = ppm_main_set_timer,
        .start = ppm_main_start,
    },
};

/**
 * @file pwm.cc
 * @author WittXie
 * @brief pwm驱动
 * @version 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../serial_led_bsp.h"
static void serial_led_button_init(void)
{
}
static void serial_led_button_deinit(void)
{
    HAL_TIM_PWM_Stop_DMA(&htim16, TIM_CHANNEL_1);
}
static void serial_led_button_start(void *pulse, uint32_t pulse_length)
{
    uint8_t ret = 0;
    ret = HAL_TIM_PWM_Start_DMA(&htim16, TIM_CHANNEL_1, (uint32_t *)pulse, pulse_length);
    if (ret != HAL_OK)
    {
        extern sled_t g_serial_led;
        ERROR("%s start error\r\n", g_serial_led.cfg.name);
    }
}
sled_t g_serial_led = {
    .cfg = {
        .name = "g_serial_led",
        .timer_bits = 16,                         // 16位定时器
        .timer_period = (295 + 595) * 240 / 1000, // 定时器周期值
        .led_num = 16,                            // 14灯珠串联, 最后两个是假灯
        .time_t0h = 295,                          // T0H = 295 ns
        .time_t0l = 595,                          // T0L = 595 ns
        .time_reset = 85000,                      // 80 000 ns
        .order = SERIAL_LED_ORDER_GRB,            // 颜色顺序: GRB
    },
    .ops = {
        .init = serial_led_button_init,
        .deinit = serial_led_button_deinit,
        .start = serial_led_button_start,
    },
};

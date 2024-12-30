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
#include "./../gpio_bsp.h"

#define MAKE_PWM(_NAME, _HTIM_X, _TIM_CHANNEL_X, _VALUE)                                                         \
    static uint32_t __##_NAME##_read(void)                                                                       \
    {                                                                                                            \
        extern gpio_t _NAME;                                                                                     \
        return __HAL_TIM_GET_COMPARE(&(_HTIM_X), (_TIM_CHANNEL_X)) == 0 ? 0 : (_NAME).cfg.value;                 \
    }                                                                                                            \
    static void __##_NAME##_write(uint32_t value)                                                                \
    {                                                                                                            \
        __HAL_TIM_SET_COMPARE(&(_HTIM_X), (_TIM_CHANNEL_X), __HAL_TIM_GET_AUTORELOAD(&(_HTIM_X)) * value / 100); \
    }                                                                                                            \
    static void __##_NAME##_init(void)                                                                           \
    {                                                                                                            \
        __HAL_TIM_CLEAR_FLAG(&(_HTIM_X), TIM_FLAG_UPDATE);                                                       \
        HAL_TIM_PWM_Start(&(_HTIM_X), (_TIM_CHANNEL_X));                                                         \
        __##_NAME##_write(0);                                                                                    \
    }                                                                                                            \
    gpio_t _NAME = {                                                                                             \
        .cfg = {                                                                                                 \
            .name = DEFINE_TO_STR(_NAME),                                                                        \
            .value = (_VALUE),                                                                                   \
        },                                                                                                       \
        .ops = {                                                                                                 \
            .init = &__##_NAME##_init,                                                                           \
            .read = &__##_NAME##_read,                                                                           \
            .write = &__##_NAME##_write,                                                                         \
        },                                                                                                       \
    }

// PWM, value: 0-100
MAKE_PWM(g_pwm_vibration, htim2, TIM_CHANNEL_1, 100); // vibration: htim2_ch1
MAKE_PWM(g_pwm_lcd_light, htim2, TIM_CHANNEL_2, 100); // lcd_light: htim2_ch2

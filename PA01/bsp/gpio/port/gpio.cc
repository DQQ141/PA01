/**
 * @file gpio.cc
 * @author WittXie
 * @brief 按键驱动
 * @version 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../gpio_bsp.h"

#define MAKE_GPIO(_NAME, _GPIO_X, _PIN_X, _TIMEGAP_SHORT_PRESS, _TIMEGAP_LONG_PRESS, _REVERSE)                                                   \
    static uint32_t __##_NAME##_read(void)                                                                                                       \
    {                                                                                                                                            \
        return (_REVERSE) ? (HAL_GPIO_ReadPin((_GPIO_X), (_PIN_X)) == GPIO_PIN_RESET) : (HAL_GPIO_ReadPin((_GPIO_X), (_PIN_X)) == GPIO_PIN_SET); \
    }                                                                                                                                            \
    static void __##_NAME##_write(uint32_t value)                                                                                                \
    {                                                                                                                                            \
        HAL_GPIO_WritePin((_GPIO_X), (_PIN_X), (_REVERSE) ? (value ? GPIO_PIN_RESET : GPIO_PIN_SET) : (value ? GPIO_PIN_SET : GPIO_PIN_RESET));  \
    }                                                                                                                                            \
                                                                                                                                                 \
    static void __##_NAME##_init(void)                                                                                                           \
    {                                                                                                                                            \
        __##_NAME##_write(0);                                                                                                                    \
    }                                                                                                                                            \
    gpio_t _NAME = {                                                                                                                             \
        .cfg = {                                                                                                                                 \
            .name = DEFINE_TO_STR(_NAME),                                                                                                        \
            .timegap_short_press = _TIMEGAP_SHORT_PRESS,                                                                                         \
            .timegap_long_press = _TIMEGAP_LONG_PRESS,                                                                                           \
            .value = 1,                                                                                                                          \
        },                                                                                                                                       \
        .ops = {                                                                                                                                 \
            .init = &__##_NAME##_init,                                                                                                           \
            .read = &__##_NAME##_read,                                                                                                           \
            .write = &__##_NAME##_write,                                                                                                         \
        },                                                                                                                                       \
    }

MAKE_GPIO(g_out_power, GPIOE, GPIO_PIN_3, 0, 0, false);              // 电源开关: PE3，高电平表示开启
MAKE_GPIO(g_btn_slw_left, GPIOA, GPIO_PIN_2, 100000, 100000, true);  // 左自锁开关: PA2
MAKE_GPIO(g_btn_roller, GPIOG, GPIO_PIN_13, 30000, 300000, true);    // 滚轮按键: PG13
MAKE_GPIO(g_btn_back_left, GPIOG, GPIO_PIN_3, 30000, 300000, true);  // 左背部按键: PG3
MAKE_GPIO(g_btn_back_right, GPIOG, GPIO_PIN_2, 30000, 300000, true); // 右背部按键: PG2
MAKE_GPIO(g_btn_right, GPIOF, GPIO_PIN_8, 100000, 600000, true);     // 右弹簧按钮: PF8
MAKE_GPIO(g_btn_power, GPIOE, GPIO_PIN_6, 30000, 300000, true);      // 电源按钮: PE6，低电平表示按下
MAKE_GPIO(g_btn_dfu, GPIOE, GPIO_PIN_5, 30000, 300000, false);       // 电源按钮: PE5，高电平表示按下，不用翻转IO

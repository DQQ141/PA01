/**
 * @file rgb.cc
 * @author WittXie
 * @brief RGB LED驱动
 * @version 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../serial_led/serial_led_bsp.h"
#include "./../gpio_bsp.h"

#define MAKE_RGB(_NAME, _LED, _INDEX, _VALUE)                         \
    static uint32_t __##_NAME##_read(void)                            \
    {                                                                 \
        return (_LED).color[(_INDEX)].value;                          \
    }                                                                 \
    static void __##_NAME##_write(uint32_t value)                     \
    {                                                                 \
        serial_led_color_set(&(_LED), (_INDEX), (sled_color_t)value); \
    }                                                                 \
    static void __##_NAME##_init(void)                                \
    {                                                                 \
        __##_NAME##_write(false);                                     \
    }                                                                 \
    gpio_t _NAME = {                                                  \
        .cfg = {                                                      \
            .name = DEFINE_TO_STR(_NAME),                             \
            .value = (_VALUE),                                        \
        },                                                            \
        .ops = {                                                      \
            .init = &__##_NAME##_init,                                \
            .read = &__##_NAME##_read,                                \
            .write = &__##_NAME##_write,                              \
        },                                                            \
    }

#define MAKE_RGB_DOUBLE(_NAME, _LED, _INDEX1, _INDEX2, _VALUE)         \
    static uint32_t __##_NAME##_read(void)                             \
    {                                                                  \
        return (_LED).color[(_INDEX1)].value;                          \
    }                                                                  \
    static void __##_NAME##_write(uint32_t value)                      \
    {                                                                  \
        serial_led_color_set(&(_LED), (_INDEX1), (sled_color_t)value); \
        serial_led_color_set(&(_LED), (_INDEX2), (sled_color_t)value); \
    }                                                                  \
    static void __##_NAME##_init(void)                                 \
    {                                                                  \
        __##_NAME##_write(false);                                      \
    }                                                                  \
    gpio_t _NAME = {                                                   \
        .cfg = {                                                       \
            .name = DEFINE_TO_STR(_NAME),                              \
            .value = (_VALUE),                                         \
        },                                                             \
        .ops = {                                                       \
            .init = &__##_NAME##_init,                                 \
            .read = &__##_NAME##_read,                                 \
            .write = &__##_NAME##_write,                               \
        },                                                             \
    }

MAKE_RGB_DOUBLE(g_rgb_power, g_serial_led, 14, 15, 0xFFFFFF); // led_power 假灯

MAKE_RGB(g_rgb_left1, g_serial_led, 12, 0xFFFFFF);  // led_left1
MAKE_RGB(g_rgb_left2, g_serial_led, 13, 0xFFFFFF);  // led_left2
MAKE_RGB(g_rgb_power1, g_serial_led, 8, 0xFFFFFF);  // led_power1
MAKE_RGB(g_rgb_power2, g_serial_led, 9, 0xFFFFFF);  // led_power2
MAKE_RGB(g_rgb_11, g_serial_led, 5, 0xFFFFFF);      // led_11
MAKE_RGB(g_rgb_12, g_serial_led, 4, 0xFFFFFF);      // led_12
MAKE_RGB(g_rgb_21, g_serial_led, 7, 0xFFFFFF);      // led_21
MAKE_RGB(g_rgb_22, g_serial_led, 6, 0xFFFFFF);      // led_22
MAKE_RGB(g_rgb_31, g_serial_led, 1, 0xFFFFFF);      // led_31
MAKE_RGB(g_rgb_32, g_serial_led, 0, 0xFFFFFF);      // led_32
MAKE_RGB(g_rgb_41, g_serial_led, 3, 0xFFFFFF);      // led_41
MAKE_RGB(g_rgb_42, g_serial_led, 2, 0xFFFFFF);      // led_42
MAKE_RGB(g_rgb_right2, g_serial_led, 11, 0xFFFFFF); // led_right2
MAKE_RGB(g_rgb_right1, g_serial_led, 10, 0xFFFFFF); // led_right1

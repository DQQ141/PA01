/**
 * @file serial_led.h
 * @author WittXie
 * @brief 串行灯珠驱动
 * @version 0.1
 * @date 2024-09-24
 * @note 这是一个串行灯珠的通用型驱动, 占用CPU较多
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifndef MALLOC
#define MALLOC(_size) malloc(_size)
#define FREE(_pv) free(_pv)
#endif

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

typedef union __serial_led_color_union
{
    uint32_t value;   // 灯珠颜色值 value = r << 16 | g << 8 | b
    uint8_t array[4]; // 灯珠颜色数组 array[2] = r, array[1] = g, array[0] = b
    struct
    {
        uint8_t b; // 0-255 蓝色
        uint8_t g; // 0-255 绿色
        uint8_t r; // 0-255 红色
        uint8_t not_used;
    } rgb;
} sled_color_t;

typedef enum __serial_led_order_enum
{
    SERIAL_LED_ORDER_RGB = 0,
    SERIAL_LED_ORDER_RBG,
    SERIAL_LED_ORDER_GRB,
    SERIAL_LED_ORDER_GBR,
    SERIAL_LED_ORDER_BRG,
    SERIAL_LED_ORDER_BGR,
    SERIAL_LED_ORDER_MAX
} serial_led_order_et;

typedef struct __serial_led
{
    // 参数
    struct
    {
        const char *name;          // 名称
        uint32_t timer_bits;       // 定时器位数（16位/32位）
        uint32_t timer_period;     // 定时器周期值, 即ARR
        uint32_t led_num;          // 灯珠数量
        uint32_t time_reset;       // 复位时间，单位ns
        uint32_t time_t0h;         // T0H时间，单位ns
        uint32_t time_t0l;         // T0L时间，单位ns
        serial_led_order_et order; // 颜色顺序
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);                                // 初始化
        void (*deinit)(void);                              // 关闭
        void (*start)(void *pulse, uint32_t pulse_length); // 启动
    } ops;

    // 标志
    union
    {
        uint8_t value;
        struct
        {
            bool is_inited : 1; // 是否已初始化
        };
    } flag;

    // 数据
    void *pulse;                 // 灯珠数据
    uint32_t pulse_length;       // 数据长度, 单位：个
    uint32_t pulse_reset_length; // RESET所需要的数据，单位：byte
    uint32_t pulse_t0h;          // T0H对应的数据值
    uint32_t pulse_t0l;          // T0L对应的数据值
    sled_color_t *color;         // 灯珠组颜色
} sled_t;

/**
 * @brief 初始化串联灯珠
 *
 * @param led 指向sled_t结构的指针，该结构包含了串联灯珠操作所需的接口和数据。
 */
void serial_led_init(sled_t *led);

/**
 * @brief 关闭串联灯珠
 *
 * @param led 指向sled_t结构的指针
 */
void serial_led_deinit(sled_t *led);

/**
 * @brief 打印串联灯珠参数
 *
 * @param led 指向sled_t结构的指针
 */
void serial_led_args_print(sled_t *led);

/**
 * @brief 设置灯珠颜色
 *
 * @param led 指向sled_t结构的指针
 * @param index 灯珠索引 从0开始
 * @param color 灯珠颜色
 */
void serial_led_color_set(sled_t *led, uint32_t index, sled_color_t color);

/**
 * @brief 设置灯珠HSV颜色
 *
 * @param led 指向sled_t结构的指针
 * @param index 通道索引
 * @param hue 色相 (0-359)
 * @param saturation 饱和度 (0-255)
 * @param brightness 亮度 (0-255)
 */
void serial_led_hsv_set(sled_t *led, uint32_t index, uint16_t hue, uint8_t saturation, uint8_t brightness);

/**
 * @brief 获取灯珠颜色
 *
 * @param led 指向sled_t结构的指针
 * @param index 灯珠索引 从0开始
 * @return 灯珠颜色
 */
#define serial_led_color_get(_led, _index) ((_led)->color[_index])

/**
 * @brief 设备是否初始化
 *
 * @param led 指向sled_t结构的指针
 * @return bool true为已初始化，false为未初始化
 */
#define serial_led_is_inited(_led) ((_led)->flag.is_inited)
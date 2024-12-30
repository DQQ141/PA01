/**
 * @file gc9307c.h
 * @author WittXie
 * @brief gc9307c屏幕驱动
 * @version 0.1
 * @date 2024-09-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

#ifndef MUTEX_LOCK
#define MUTEX_LOCK(_mutex) ((bool)true)
#define MUTEX_UNLOCK(_mutex) ((void)0)
#endif

#include "./gc9307c_reg.h"

#define GC9307C_COLOR(_r, _g, _b) ((uint16_t)((((_r) & 0x1F) << 11) | (((_g) & 0x3F) << 5) | ((_b) & 0x1F)))

typedef struct __gc9307c
{
    // 配置
    struct
    {
        const char *name;
        uint16_t *buff;
        uint16_t width;
        uint16_t height;
        gc9307c_dir_t direction; // 屏幕方向
    } cfg;

    // 接口
    struct
    {
        void (*init)(void);
        void (*reset)(void);
        void (*write_command)(uint8_t cmd);
        void (*write_data)(uint8_t *buff, uint32_t length);
        void (*delay)(void); // 启动后延时
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

    void *mutex;
    uint16_t width;
    uint16_t height;

    uint16_t auto_sync_block_size;  // 每次自动同步的数据块大小
    uint32_t auto_sync_block_count; // 自动同步的数据块数量
    uint32_t auto_sync_block_index; // 自动同步的数据块索引

    uint64_t timestamp; // 当前时间戳,单位：us
    uint64_t frame_gap; // 帧间隔，单位：us
} gc9307c_t;

/**
 * @brief 初始化GC9307C显示屏
 *
 * @param lcd 指向gc9307c_t结构的指针，该结构包含了LCD操作所需的接口和数据
 */
void gc9307c_init(gc9307c_t *lcd);

/**
 * @brief GC9307C寄存器初始化
 *
 * @param lcd 指向gc9307c_t结构体的指针
 */
void gc9307c_reg_init(gc9307c_t *lcd);

/**
 * @brief GC9307C进入睡眠模式
 *
 * @param lcd 指向gc9307c_t结构体的指针
 */
void gc9307c_enter_sleep(gc9307c_t *lcd);

/**
 * @brief GC9307C退出睡眠模式
 *
 * @param lcd 指向gc9307c_t结构体的指针
 */
void gc9307c_exit_sleep(gc9307c_t *lcd);

/**
 * @brief 帧计数钩子
 *
 * @param lcd 指向gc9307c_t结构体的指针
 * @param timestamp 当前时间戳
 */
void gc9307c_frame_count_hook(gc9307c_t *lcd, uint64_t timestamp);

/**
 * @brief 清屏，填充指定颜色
 *
 * @param lcd 指向gc9307c_t结构体的指针
 * @param color 颜色值
 */
void gc9307c_clear(gc9307c_t *lcd, uint16_t color);

/**
 * @brief 将指定区域的像素数据写入到GC9307C LCD显示屏中
 *
 * @param lcd 指向GC9307C LCD控制器结构体的指针
 * @param x 写入区域的左上角x坐标
 * @param y 写入区域的左上角y坐标
 * @param x2 写入区域的右下角x坐标
 * @param y2 写入区域的右下角y坐标
 * @param buff 包含像素数据的缓冲区
 * @param length 缓冲区的长度
 */
void gc9307c_write(gc9307c_t *lcd, uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t *buff, uint32_t length);

/**
 * @brief 设备是否初始化
 *
 * @param lcd 指向GC9307C LCD控制器结构体的指针
 * @return bool true为已初始化，false为未初始化
 */
#define gc9307c_is_inited(lcd) ((lcd)->flag.is_inited)

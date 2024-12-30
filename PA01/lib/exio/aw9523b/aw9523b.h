/**
 * @file aw9523b.h
 * @author WittXie
 * @brief 拓展IO芯片AW9523B驱动
 * @version 0.1
 * @date 2024-09-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

// 通用接口
#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

#ifndef ERROR
#define ERROR(_format, ...) ((void)0)
#endif

#ifndef MUTEX_LOCK
#define MUTEX_LOCK(_mutex) ((bool)true)
#define MUTEX_UNLOCK(_mutex) ((void)0)
#endif

typedef union __aw9523b_io
{
    uint16_t value;
    uint8_t data[2]; // data[1]高位
    struct
    {
        uint8_t bit0 : 1;  // 第0位 低位
        uint8_t bit1 : 1;  // 第1位
        uint8_t bit2 : 1;  // 第2位
        uint8_t bit3 : 1;  // 第3位
        uint8_t bit4 : 1;  // 第4位
        uint8_t bit5 : 1;  // 第5位
        uint8_t bit6 : 1;  // 第6位
        uint8_t bit7 : 1;  // 第7位
        uint8_t bit8 : 1;  // 第8位
        uint8_t bit9 : 1;  // 第9位
        uint8_t bit10 : 1; // 第10位
        uint8_t bit11 : 1; // 第11位
        uint8_t bit12 : 1; // 第12位
        uint8_t bit13 : 1; // 第13位
        uint8_t bit14 : 1; // 第14位
        uint8_t bit15 : 1; // 第15位 高位
    } bits;
} aw9523b_io_ut;

#pragma pack(4)
typedef struct __aw9523b
{
    // 参数
    struct
    {
        const char *name; // 名称
        uint8_t try_cnt;  // 尝试初始化次数
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);
        void (*read)(uint8_t addr, uint8_t *buff, uint32_t length);  // 读取
        void (*write)(uint8_t addr, uint8_t *buff, uint32_t length); // 写入
        void (*delay)(void);                                         // 重启延时
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

    void *mutex; // 互斥锁
    uint8_t init_try_cnt;

    // 映射寄存器
    aw9523b_io_ut output;   // 输出寄存器
    aw9523b_io_ut mask;     // 配置寄存器
    uint8_t brightness[16]; // 亮度
    uint8_t imax;           // 最大电流

} aw9523b_t;
#pragma pack()

/**
 * @brief 初始化
 *
 * @param aw9523b 设备指针
 */
void aw9523b_init(aw9523b_t *aw9523b);

/**
 * @brief 轮询
 *
 * @param aw9523b 设备指针
 */
void aw9523b_poll(aw9523b_t *aw9523b);

/**
 * @brief 设置GPIO掩码
 *
 * @param aw9523b 设备指针
 * @uint16_t GPIO掩码，1为输入，0为输出
 */
uint16_t aw9523b_mask_read(aw9523b_t *aw9523b);

/**
 * @brief 设置GPIO掩码
 *
 * @param aw9523b 设备指针
 * @param gpio_mask GPIO掩码，1为输入，0为输出
 */
void aw9523b_mask_write(aw9523b_t *aw9523b, uint16_t gpio_mask);

/**
 * @brief 设置GPIO方向
 *
 * @param aw9523b 设备指针
 * @param pin GPIO引脚号
 * @param is_output 是否为输出
 */
void aw9523b_pin_config(aw9523b_t *aw9523b, uint8_t pin, bool is_output);

/**
 * @brief 获取GPIO方向
 *
 * @param aw9523b 设备指针
 * @param pin GPIO引脚号
 * @return true 为输出，false 为输入
 */
bool aw9523b_pin_config_get(aw9523b_t *aw9523b, uint8_t pin);

/**
 * @brief 读取GPIO值
 *
 * @param aw9523b 设备指针
 * @return gpio
 */
uint16_t aw9523b_input_read(aw9523b_t *aw9523b);  // 读取INPUT的GPIO值
uint16_t aw9523b_output_read(aw9523b_t *aw9523b); // 读取OUTPUT的GPIO值

/**
 * @brief 设置GPIO值
 *
 * @param aw9523b 设备指针
 * @param value GPIO值 16位
 */
void aw9523b_write(aw9523b_t *aw9523b, uint16_t value);

/**
 * @brief 设置GPIO PIN值
 *
 * @param aw9523b 设备指针
 * @param pin GPIO引脚号
 * @param is_high 值（true为高，false为低）
 */
void aw9523b_pin_write(aw9523b_t *aw9523b, uint8_t pin, bool is_high);

/**
 * @brief 获取GPIO值
 *
 * @param aw9523b 设备指针
 * @param pin GPIO引脚号
 * @return bool GPIO值（true为高，false为低）
 */
bool aw9523b_input_pin_read(aw9523b_t *aw9523b, uint8_t pin);
bool aw9523b_output_pin_read(aw9523b_t *aw9523b, uint8_t pin);

/**
 * @brief 设置LED亮度
 *
 * @param aw9523b 设备指针
 * @param led LED编号
 * @param brightness 亮度值（0-255）
 */
void aw9523b_brightness_write(aw9523b_t *aw9523b, uint8_t led, uint8_t brightness);
uint8_t aw9523b_brightness_read(aw9523b_t *aw9523b, uint8_t led);

/**
 * @brief 设置LED电流限制
 *
 * @param aw9523b 设备指针
 * @param imax 电流限制（0-3）
 */
void aw9523b_imax_write(aw9523b_t *aw9523b, uint8_t imax);
uint8_t aw9523b_imax_read(aw9523b_t *aw9523b);

/**
 * @brief 设备是否初始化
 *
 * @param aw9523b 设备指针
 * @return bool true为已初始化，false为未初始化
 */
#define aw9523b_is_inited(_aw9523b) ((_aw9523b)->flag.is_inited)

/**
 * @brief 设备是否正在运行
 *
 * @param aw9523b 设备指针
 * @return bool true为正在运行，false为未运行
 */
#define aw9523b_is_running(_aw9523b) ((_aw9523b)->flag.is_running)

/**
 * @file ppm.h
 * @author WittXie
 * @brief PPM驱动
 * @version 0.1
 * @date 2024-10-11
 * @note PPM发射与接收
 * 发送用PWM比较输出触发DMA，自动产生波形
 * 接收用外部中断，记录上升沿时间
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 依赖
#include "./../dds/dds.h" // 订阅机制

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

#ifndef ERROR
#define ERROR(_format, ...) ((void)0)
#endif

typedef struct __ppm
{
    // 参数
    struct
    {
        const char *name;      // 名称
        uint32_t timer_bits;   // 定时器位数（16位/32位）
        uint32_t timer_clk;    // 定时器时钟
        uint32_t period_us;    // 周期值, 单位：us
        uint32_t channels;     // 通道数
        uint32_t n_t0;         // T0L所需的周期, 脉宽 = n_t0 * period_us
        uint32_t n_t1;         // T1L所需的周期, 脉宽 = n_t1 * period_us
        uint32_t tolerance_us; // 脉宽容差, 单位：us
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);                                                 // 初始化
        void (*deinit)(void);                                               // 关闭
        void (*set_timer)(uint32_t timer_prescaler, uint32_t timer_period); // 设置定时器
        void (*start)(void *pulse, uint32_t pulse_length);                  // 启动一帧数据的传输
    } ops;

    uint32_t data_length; // 存储通道数据的长度, 单位：byte

    // 发射
    struct
    {
        void *pulse;               // PPM数据
        uint32_t pulse_length_max; // 脉冲最大长度, 单位：个
        uint8_t *send_data;        // 通道数据；根据通道数，分配内存
        uint8_t *last_send_data;   // 上次发送的通道数据
        uint32_t timer_prescaler;  // 定时器预分频器
        uint32_t timer_period;     // 定时器周期
        uint32_t pulse_h;          // 高电平值
        uint32_t pulse_l;          // 低电平值
        uint32_t pulse_length;     // 脉冲的长度
        uint32_t n_bit_max;        // 单个bit最大脉冲宽度
    } send;

    // 接收
    struct
    {
        uint8_t *receive_data;      // 通道数据；根据通道数，分配内存
        uint8_t *last_receive_data; // 上次接收到的通道数据
        uint64_t last_timestamp_us; // 上一次接收到数据的时间戳，单位：us
        uint32_t bit_cnt;           // 当前bit计数
        uint32_t time_bit_max;      // 单个bit最大脉冲时间
        uint32_t time_t0;           // t0脉冲时间
        uint32_t time_t1;           // t1脉冲时间
    } receive;

    // 标志
    union
    {
        uint8_t value;
        struct
        {
            bool is_inited : 1;  // 是否已初始化
            bool is_running : 1; // 是否正在运行
        };
    } flag;

    dds_topic_t RECEIVED_DATA_CHANGED; // 发射时触发
    dds_topic_t SEND_DATA_CHANGED;     // 发射时触发

} ppm_t;

/**
 * @brief 初始化串联PPM
 *
 * @param ppm 指向ppm_t结构的指针，该结构包含了串联PPM操作所需的接口和数据
 */
void ppm_init(ppm_t *ppm);

/**
 * @brief 关闭串联PPM
 *
 * @param ppm 指向ppm_t结构的指针
 */
void ppm_deinit(ppm_t *ppm);

/**
 * @brief 论询
 *
 * @param ppm 指向ppm_t结构的指针
 */
void ppm_poll(ppm_t *ppm);

/**
 * @brief 设置定时器周期
 *
 * @param ppm 指向ppm_t结构的指针
 * @param period_us  周期值, 单位：us
 */
void ppm_period_set(ppm_t *ppm, uint32_t period_us);

/**
 * @brief 写入数据
 *
 * @param ppm 指向ppm_t结构的指针
 * @param data 数据, 数据长度由 ppm->data_length 决定
 */
void ppm_write(ppm_t *ppm, void *data);

/**
 * @brief 写入通道数据
 *
 * @param ppm 指向ppm_t结构的指针
 * @param index 通道索引
 * @param valid 数据有效
 */
void ppm_channel_write(ppm_t *ppm, uint8_t index, bool valid);

/**
 * @brief 读取PPM接收时的上升沿外部中断时间戳
 *
 * @param ppm 指向ppm_t结构的指针
 * @param timestamp_us 通道时间戳
 */
void ppm_read_hook(ppm_t *ppm, uint64_t timestamp_us);

/**
 * @brief 读取通道数据
 *
 * @param ppm 指向ppm_t结构的指针
 * @param index 通道索引
 * @return true 数据1
 * @return false 数据0
 */
bool ppm_send_channel_read(ppm_t *ppm, uint8_t index);
bool ppm_receive_channel_read(ppm_t *ppm, uint8_t index);

/**
 * @brief 设备是否初始化
 *
 * @param ppm 指向ppm_t结构的指针
 * @return bool true为已初始化，false为未初始化
 */
#define ppm_is_inited(ppm) ((ppm)->flag.is_inited)

/**
 * @brief 设备是否运行
 *
 * @param ppm 指向ppm_t结构的指针
 * @return bool true为已运行，false为未运行
 */
#define ppm_is_running(ppm) ((ppm)->flag.is_running)

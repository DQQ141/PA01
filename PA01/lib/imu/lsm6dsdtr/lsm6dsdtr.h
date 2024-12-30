/**
 * @file lsm6dsdtr.h
 * @author WittXie
 * @brief 陀螺仪传感器驱动
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

// 依赖
#include "./../../dds/dds.h" // 订阅机制

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

#ifndef TIMESTAMP_US_GET
#error "please define TIMESTAMP_US_GET"
#define TIMESTAMP_US_GET() 0
#endif

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

typedef struct __lsm6dsdtr_config_t
{
    // ctrl1_xl: 加速度计 ODR 和 FS
    union
    {
        uint8_t ctrl1_xl;
        struct
        {
            uint8_t not_used_01 : 2; // 保留位
            uint8_t fs_xl : 2;       // 加速度计全量程选择 (2位, 0x00: ±2g, 0x01: ±16g, 0x02: ±4g, 0x03: ±8g)
            uint8_t odr_xl : 4;      // 加速度计输出数据速率 (4位, 0x00: power down, 0x01: 12.5 Hz, 0x02: 26 Hz, 0x03: 52 Hz, 0x04: 104 Hz, 0x05: 208 Hz, 0x06: 416 Hz, 0x07: 833 Hz, 0x08: 1.66 kHz, 0x09: 3.33 kHz, 0x0A: 6.66 kHz)
        } ctrl1_xl_bits;
    };

    // ctrl2_g: 陀螺仪 ODR 和 FS
    union
    {
        uint8_t ctrl2_g;
        struct
        {
            uint8_t not_used_01 : 2; // 保留位
            uint8_t fs_g : 2;        // 陀螺仪全量程选择 (2位, 0x00: ±250 dps, 0x01: ±500 dps, 0x02: ±1000 dps, 0x03: ±2000 dps)
            uint8_t odr_g : 4;       // 陀螺仪输出数据速率 (4位, 0x00: power down, 0x01: 12.5 Hz, 0x02: 26 Hz, 0x03: 52 Hz, 0x04: 104 Hz, 0x05: 208 Hz, 0x06: 416 Hz, 0x07: 833 Hz, 0x08: 1.66 kHz, 0x09: 3.33 kHz, 0x0A: 6.66 kHz)
        } ctrl2_g_bits;
    };

    // ctrl3_c: 常规控制寄存器
    union
    {
        uint8_t ctrl3_c;
        struct
        {
            uint8_t sw_reset : 1;    // 重启 (1位, 0: 正常工作, 1: 重启)
            uint8_t not_used_01 : 1; // 保留位
            uint8_t if_inc : 1;      // 自动递增寄存器地址 (1位, 0: 禁用, 1: 使能)
            uint8_t sim : 1;         // SPI模式选择 (1位, 0: 4-wire, 1: 3-wire)
            uint8_t not_used_02 : 2; // 保留位
            uint8_t bdu : 1;         // 数据更新阻塞 (1位, 0: 连续更新, 1: 阻塞更新)
            uint8_t ram_clear : 1;   // 重启 (1位, 0: 正常工作, 1: 清除内存)
        } ctrl3_c_bits;
    };

    // ctrl4_c: 加速度计和陀螺仪使能
    union
    {
        uint8_t ctrl4_c;
        struct
        {
            uint8_t not_used_01 : 2;  // 保留位
            uint8_t i2c_disable : 1;  // I2C 接口禁用 (1位, 0: 启用, 1: 禁用)
            uint8_t drdy_mask : 1;    // 数据就绪信号屏蔽 (1位, 0: 禁用, 1: 使能)
            uint8_t not_used_02 : 1;  // 保留位
            uint8_t int2_on_int1 : 1; // 将 INT2 中断信号路由到 INT1 引脚 (1位, 0: 禁用, 1: 使能)
            uint8_t sleep_g : 1;      // 陀螺仪睡眠模式使能 (1位, 0: 禁用, 1: 使能)
            uint8_t not_used_03 : 1;  // 保留位
        } ctrl4_c_bits;
    };

    // ctrl5_c: 自检和数据舍入配置
    union
    {
        uint8_t ctrl5_c;
        struct
        {
            uint8_t st_xl : 2;       // 加速度计自检 (2位, 0x00: 禁用, 0x01: 正自检, 0x02: 负自检)
            uint8_t st_g : 2;        // 陀螺仪自检 (2位, 0x00: 禁用, 0x01: 正自检, 0x02: 负自检)
            uint8_t not_used_01 : 1; // 保留位
            uint8_t rounding : 2;    // 数据舍入模式 (2位, 0x00: no rounding; 0x01: accelerometer only; 0x02: gyroscope only; 0x03: gyroscope + accelerometer)
            uint8_t not_used_02 : 1; // 保留位
        } ctrl5_c_bits;
    };

    // ctrl6_c: 高性能模式配置
    union
    {
        uint8_t ctrl6_c;
        struct
        {
            uint8_t ftype : 3;          // 低通滤波器类型 (2位, 0x00: LPF1, 0x01: LPF2, 0x02: HPF, 0x03: LPF2)
            uint8_t usr_off_on_out : 1; // 用户偏移补偿 (1位, 0: 禁用, 1: 使能)
            uint8_t xl_hm_mode : 1;     // 加速度计高性能模式 (1位, 0: 禁用, 1: 使能)
            uint8_t den_mode : 2;       // 数据使能模式 (3位, 0x00: 禁用, 0x01: 到 XL 数据, 0x02: 到 G 数据, 0x03: 到 XL 和 G 数据, 0x04: 到温度数据)
            uint8_t trig_en : 1;        // 触发使能 (1位, 0: 禁用, 1: 使能)
        } ctrl6_c_bits;
    };

    // ctrl7_g: 陀螺仪滤波器配置
    union
    {
        uint8_t ctrl7_g;
        struct
        {
            uint8_t ois_on : 1;         // 陀螺仪自检使能 (1位, 0: 禁用, 1: 使能)
            uint8_t usr_off_on_out : 1; // 用户偏移补偿 (1位, 0: 禁用, 1: 使能)
            uint8_t ois_on_en : 1;      // 陀螺仪自检使能 (1位, 0: 禁用, 1: 使能)
            uint8_t not_used_01 : 1;    // 保留位
            uint8_t hpm_g : 2;          // 高通滤波器模式 (2位, 0x00: 正常模式, 0x01: 参考信号, 0x02: 自动复位)
            uint8_t hp_en_g : 1;        // 高通滤波器使能 (1位, 0: 禁用, 1: 使能)
            uint8_t g_hm_mode : 1;      // 陀螺仪高性能模式 (1位, 0: 禁用, 1: 使能)
        } ctrl7_g_bits;
    };

    // ctrl8_xl: 加速度计滤波器配置
    union
    {
        uint8_t ctrl8_xl;
        struct
        {
            uint8_t low_pass_on_6d : 1;    // 6D 方向检测低通滤波器 (1位, 0: 禁用, 1: 使能)
            uint8_t not_used_01 : 1;       // 保留位
            uint8_t hp_slope_xl_en : 1;    // 高通或斜率滤波器 (1位, 0: 禁用, 1: 使能)
            uint8_t fastsettl_mode_xl : 1; // 快速设置模式 (1位, 0: 禁用, 1: 使能)
            uint8_t hp_ref_mode_xl : 1;    // 高通或斜率滤波器 (1位, 0: 禁用, 1: 使能)
            uint8_t hpcf_xl : 3;           // 高通滤波器截止频率(3位, 0x00:208 Hz, 0x01:83.3HZ, 0x02:41.6HZ, 0x03:18.5HZ, 0x04:8.3HZ, 0x05:4.15HZ, 0x06:2.08HZ, 0x07:1.04HZ)
        } ctrl8_xl_bits;
    };

    // ctrl9_xl: 加速度计中断和DRDY配置
    union
    {
        uint8_t ctrl9_xl;
        struct
        {
            uint8_t not_used_01 : 1;       // 保留位
            uint8_t xl_self_test_sign : 2; // 加速度计自检符号 (2位, 0x00: 正常模式, 0x01: 正自检, 0x02: 负自检)
            uint8_t xl_self_test_en : 1;   // 加速度计自检使能 (1位, 0: 禁用, 1: 使能)
            uint8_t den_x : 1;             // X轴DEN值存储 (1位, 0: 不存储, 1: 存储)
            uint8_t den_y : 1;             // Y轴DEN值存储 (1位, 0: 不存储, 1: 存储)
            uint8_t den_z : 1;             // Z轴DEN值存储 (1位, 0: 不存储, 1: 存储)
        } ctrl9_xl_bits;
    };

    // ctrl10_c: 功能和I3C禁用配置
    union
    {
        uint8_t ctrl10_c;
        struct
        {
            uint8_t timestamp_en : 1; // 时间戳使能 (1位, 0: 禁用, 1: 使能)
            uint8_t not_used_01 : 1;  // 保留位
            uint8_t i3c_disable : 1;  // I3C接口禁用 (1位, 0: 启用, 1: 禁用)
            uint8_t not_used_02 : 5;  // 保留位
        } ctrl10_c_bits;
    };
} lsm6dsdtr_config_t;

typedef struct __lsm6dsdtr
{
    // 参数
    struct
    {
        const char *name;          // 名称
        lsm6dsdtr_config_t config; // 芯片配置
        float filter_alpha;        // 低通滤波系数 0~1
        float bias;                // 角速度偏置(即0漂)
        uint8_t try_cnt;           // 初始化尝试次数
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);
        void (*read)(uint8_t addr, uint8_t *buff, uint32_t length);  // 读取
        void (*write)(uint8_t addr, uint8_t *buff, uint32_t length); // 写入
        void (*delay)(void);                                         // 重启延时
    } ops;

    // 数据
    struct
    {
        // 加速度计数据 (范围: ±2g, ±4g, ±8g, ±16g)
        struct
        {
            int16_t x; // X轴加速度 (单位: mg)
            int16_t y; // Y轴加速度 (单位: mg)
            int16_t z; // Z轴加速度 (单位: mg)
        } accel;

        // 陀螺仪数据 (范围: ±125dps, ±250dps, ±500dps, ±1000dps, ±2000dps)
        struct
        {
            int16_t x; // X轴角速度 (单位: mdps)
            int16_t y; // Y轴角速度 (单位: mdps)
            int16_t z; // Z轴角速度 (单位: mdps)
        } gyro;

        // 角度数据 (范围: -180° to +180°)
        struct
        {
            float x; // X轴角度 (单位: °)
            float y; // Y轴角度 (单位: °)
            float z; // Z轴角度 (单位: °)
        } angle;

        float gyro_sensitivity; // 陀螺仪灵敏度
        float angle_z_accum;    // z轴角度累加值

        // 温度数据 (范围: -40°C to +85°C)
        int16_t temperature; // 温度 (单位: °C)

        // 时间戳 (范围: 0 to 4,294,967,295)
        uint64_t timestamp; // 时间戳 (单位: 秒)
    } data;

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
    uint8_t init_try_cnt; // 初始化尝试次数

    // 订阅
    dds_topic_t UPDATE; // 更新

} lsm6dsdtr_t;

/**
 * @brief 初始化
 *
 * @param lsm6dsdtr 设备指针
 * @param config 配置
 */
void lsm6dsdtr_init(lsm6dsdtr_t *lsm6dsdtr);

/**
 * @brief 轮询
 *
 * @param lsm6dsdtr 设备指针
 */
void lsm6dsdtr_poll(lsm6dsdtr_t *lsm6dsdtr);

/**
 * @brief 打印
 *
 * @param lsm6dsdtr 设备指针
 */
void lsm6dsdtr_args_print(lsm6dsdtr_t *lsm6dsdtr);

/**
 * @brief 设备是否初始化
 *
 * @param lsm6dsdtr 设备指针
 * @return bool true为已初始化，false为未初始化
 */
#define lsm6dsdtr_is_inited(lsm6dsdtr) ((lsm6dsdtr)->flag.is_inited)

/**
 * @brief 设备是否正在运行
 *
 * @param lsm6dsdtr 设备指针
 * @return bool true为正在运行，false为未运行
 */
#define lsm6dsdtr_is_running(lsm6dsdtr) ((lsm6dsdtr)->flag.is_running)

// 默认配置
#include "./default_config.h"

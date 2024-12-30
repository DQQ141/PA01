/**
 * @file default_config.h
 * @author WittXie
 * @brief 默认配置
 * @version 0.1
 * @date 2024-10-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#define LSM6DSDTR_LOW_CONFIG                                    \
    {                                                           \
        .ctrl1_xl_bits = {                                      \
            .fs_xl = 0x00, /* ±2g */                           \
            .odr_xl = 0x03 /* 52 Hz */                          \
        },                                                      \
        .ctrl2_g_bits = {                                       \
            .fs_g = 0x00, /* ±250 dps */                       \
            .odr_g = 0x02 /* 52 Hz */                           \
        },                                                      \
        .ctrl3_c_bits = {                                       \
            .if_inc = 1, /* 自动递增寄存器地址 */      \
        },                                                      \
        .ctrl7_g_bits = {                                       \
            .hpm_g = 0x01, /* 高性能模式 */                \
            .hp_en_g = 1,  /* 启用高通滤波器 */          \
            .g_hm_mode = 1 /* 启用陀螺仪高性能模式 */ \
        },                                                      \
    }

#define LSM6DSDTR_HIGH_CONFIG                                   \
    {                                                           \
        .ctrl1_xl_bits = {                                      \
            .fs_xl = 0x01, /* ±16g */                          \
            .odr_xl = 0x09 /* 3.33 KHz */                       \
        },                                                      \
        .ctrl2_g_bits = {                                       \
            .fs_g = 0x03, /* ±2000 dps */                      \
            .odr_g = 0x09 /* 3.33 KHz */                        \
        },                                                      \
        .ctrl3_c_bits = {                                       \
            .if_inc = 1, /* 自动递增寄存器地址 */      \
        },                                                      \
        .ctrl7_g_bits = {                                       \
            .hpm_g = 0,    /* 高性能模式 */                \
            .hp_en_g = 0,  /* 启用高通滤波器 */          \
            .g_hm_mode = 0 /* 启用陀螺仪高性能模式 */ \
        },                                                      \
    }

#define LSM6DSDTR_DEFAULT_CONFIG LSM6DSDTR_LOW_CONFIG

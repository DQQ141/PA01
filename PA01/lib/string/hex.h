/**
 * @file hex.h
 * @author WittXie
 * @brief 进制处理
 * @version 0.1
 * @date 2023-05-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 取任意字节
 * @param _num 十六进制数
 * @param _n 字节序号 0,最低字节
 */
#define hex_byte(_num, _n) (((_num) >> (8 * (_n))) & 0xFF)

/**
 * @brief 合并字节
 */
#define hex_combine2(_buff) ((((_buff)[0] & 0xFF) << 8) | ((_buff)[1] & 0xFF))

#define hex_combine4(_buff) ((((_buff)[0] & 0xFF) << 24) | \
                             (((_buff)[1] & 0xFF) << 16) | \
                             (((_buff)[2] & 0xFF) << 8) |  \
                             ((_buff)[3] & 0xFF))

#define hex_combine8(_buff) ((((_buff)[0] & 0xFF) << 56) | \
                             (((_buff)[1] & 0xFF) << 48) | \
                             (((_buff)[2] & 0xFF) << 40) | \
                             (((_buff)[3] & 0xFF) << 32) | \
                             (((_buff)[4] & 0xFF) << 24) | \
                             (((_buff)[5] & 0xFF) << 16) | \
                             (((_buff)[6] & 0xFF) << 8) |  \
                             (((_buff)[7] & 0xFF)))

/**
 * @brief 拆分字节
 *
 */
#define hex_split2(_buff, _num)            \
    {                                      \
        (_buff)[0] = ((_num) >> 8) & 0xFF; \
        (_buff)[1] = (_num) & 0xFF;        \
    }
#define hex_split4(_buff, _num)             \
    {                                       \
        (_buff)[0] = ((_num) >> 24) & 0xFF; \
        (_buff)[1] = ((_num) >> 16) & 0xFF; \
        (_buff)[2] = ((_num) >> 8) & 0xFF;  \
        (_buff)[3] = (_num) & 0xFF;         \
    }
#define hex_split8(_buff, _num)             \
    {                                       \
        (_buff)[0] = ((_num) >> 56) & 0xFF; \
        (_buff)[1] = ((_num) >> 48) & 0xFF; \
        (_buff)[2] = ((_num) >> 40) & 0xFF; \
        (_buff)[3] = ((_num) >> 32) & 0xFF; \
        (_buff)[4] = ((_num) >> 24) & 0xFF; \
        (_buff)[5] = ((_num) >> 16) & 0xFF; \
        (_buff)[6] = ((_num) >> 8) & 0xFF;  \
        (_buff)[7] = (_num) & 0xFF;         \
    }

typedef union
{
    uint32_t u32;
    uint16_t u16[2];
    uint8_t u8[4];
    float f;
} data32_t;

typedef union
{
    uint64_t u64;
    uint32_t u32[2];
    uint16_t u16[4];
    uint8_t u8[8];
    float f[2];
    double d;
} data64_t;

/**
 * @brief 位读写
 *
 */
#define bit_read(_reg, _bit) (((_reg) >> (_bit)) & 1)
#define bit_set(_reg, _bit) (_reg | (1 << (_bit)))
#define bit_reset(_reg, _bit) (_reg & ~(1 << (_bit)))

/**
 * @brief 数组长度获取
 *
 */
#define countof(array) (sizeof(array) / sizeof(array[0]))

/**
 * @file crc.h
 * @author WittXie
 * @brief 通用CRC校验模块
 * @version 0.1
 * @date 2024-08-22
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef MALLOC
#define MALLOC(_size) malloc(_size)
#define FREE(_pv) free(_pv)
#endif

#ifndef PRINT
#define PRINT(_format, ...) ((void)0)
#endif

// 常见多项式组合
// 8-bit CRC
#define CRC_CFG_CRC8 .polynomial = 0x07, .initial_value = 0x00, .final_xor_value = 0x00, .reflect_input = false, .reflect_output = false, .crc_width = 8           // CRC-8
#define CRC_CFG_CRC8_SAE_J1850 .polynomial = 0x1D, .initial_value = 0xFF, .final_xor_value = 0xFF, .reflect_input = false, .reflect_output = false, .crc_width = 8 // CRC-8/SAE-J1850

// 16-bit CRC
#define CRC_CFG_IBM .polynomial = 0x8005, .initial_value = 0x0000, .final_xor_value = 0x0000, .reflect_input = true, .reflect_output = true, .crc_width = 16           // CRC-16/IBM (CRC-16)
#define CRC_CFG_MAXIM .polynomial = 0x8005, .initial_value = 0x0000, .final_xor_value = 0xFFFF, .reflect_input = true, .reflect_output = true, .crc_width = 16         // CRC-16/MAXIM
#define CRC_CFG_USB .polynomial = 0x8005, .initial_value = 0xFFFF, .final_xor_value = 0xFFFF, .reflect_input = true, .reflect_output = true, .crc_width = 16           // CRC-16/USB
#define CRC_CFG_MODBUS .polynomial = 0x8005, .initial_value = 0xFFFF, .final_xor_value = 0x0000, .reflect_input = true, .reflect_output = true, .crc_width = 16        // CRC-16/MODBUS
#define CRC_CFG_CCITT_FALSE .polynomial = 0x1021, .initial_value = 0xFFFF, .final_xor_value = 0x0000, .reflect_input = false, .reflect_output = false, .crc_width = 16 // CRC-16/CCITT-FALSE
#define CRC_CFG_AUG_CCITT .polynomial = 0x1021, .initial_value = 0x1D0F, .final_xor_value = 0x0000, .reflect_input = false, .reflect_output = false, .crc_width = 16   // CRC-16/AUG-CCITT
#define CRC_CFG_XMODEM .polynomial = 0x1021, .initial_value = 0x0000, .final_xor_value = 0x0000, .reflect_input = false, .reflect_output = false, .crc_width = 16      // CRC-16/XMODEM
#define CRC_CFG_DNP .polynomial = 0x3D65, .initial_value = 0x0000, .final_xor_value = 0xFFFF, .reflect_input = true, .reflect_output = true, .crc_width = 16           // CRC-16/DNP
#define CRC_CFG_KERMIT .polynomial = 0x1021, .initial_value = 0x0000, .final_xor_value = 0x0000, .reflect_input = true, .reflect_output = true, .crc_width = 16        // CRC-16/KERMIT
#define CRC_CFG_X25 .polynomial = 0x1021, .initial_value = 0xFFFF, .final_xor_value = 0xFFFF, .reflect_input = true, .reflect_output = true, .crc_width = 16           // CRC-16/X25
#define CRC_CFG_MCRF4XX .polynomial = 0x1021, .initial_value = 0xFFFF, .final_xor_value = 0x0000, .reflect_input = true, .reflect_output = true, .crc_width = 16       // CRC-16/MCRF4XX
#define CRC_CFG_T10_DIF .polynomial = 0x8BB7, .initial_value = 0x0000, .final_xor_value = 0x0000, .reflect_input = false, .reflect_output = false, .crc_width = 16     // CRC-16/T10-DIF
#define CRC_CFG_BUYPASS .polynomial = 0x8005, .initial_value = 0x0000, .final_xor_value = 0x0000, .reflect_input = false, .reflect_output = false, .crc_width = 16     // CRC-16/BUYPASS
#define CRC_CFG_DECT_R .polynomial = 0x0589, .initial_value = 0x0000, .final_xor_value = 0x0001, .reflect_input = false, .reflect_output = false, .crc_width = 16      // CRC-16/DECT-R
#define CRC_CFG_DECT_X .polynomial = 0x0589, .initial_value = 0x0000, .final_xor_value = 0x0000, .reflect_input = false, .reflect_output = false, .crc_width = 16      // CRC-16/DECT-X
#define CRC_CFG_EN_13757 .polynomial = 0x3D65, .initial_value = 0x0000, .final_xor_value = 0xFFFF, .reflect_input = false, .reflect_output = false, .crc_width = 16    // CRC-16/EN-13757
#define CRC_CFG_GENIBUS .polynomial = 0x1021, .initial_value = 0xFFFF, .final_xor_value = 0xFFFF, .reflect_input = false, .reflect_output = false, .crc_width = 16     // CRC-16/GENIBUS
#define CRC_CFG_TMS37157 .polynomial = 0x1021, .initial_value = 0x89EC, .final_xor_value = 0x0000, .reflect_input = true, .reflect_output = true, .crc_width = 16      // CRC-16/TMS37157
#define CRC_CFG_EPC .polynomial = 0x1021, .initial_value = 0xFFFF, .final_xor_value = 0xFFFF, .reflect_input = false, .reflect_output = false, .crc_width = 16         // CRC-16/EPC

// 32-bit CRC
#define CRC_CFG_CRC32 .polynomial = 0x04C11DB7, .initial_value = 0xFFFFFFFF, .final_xor_value = 0xFFFFFFFF, .reflect_input = true, .reflect_output = true, .crc_width = 32         // CRC-32
#define CRC_CFG_CRC32C .polynomial = 0x1EDC6F41, .initial_value = 0xFFFFFFFF, .final_xor_value = 0xFFFFFFFF, .reflect_input = true, .reflect_output = true, .crc_width = 32        // CRC-32C (Castagnoli)
#define CRC_CFG_CRC32_MPEG2 .polynomial = 0x04C11DB7, .initial_value = 0xFFFFFFFF, .final_xor_value = 0x00000000, .reflect_input = false, .reflect_output = false, .crc_width = 32 // CRC-32/MPEG-2
#define CRC_CFG_CRC32_BZIP2 .polynomial = 0x04C11DB7, .initial_value = 0xFFFFFFFF, .final_xor_value = 0xFFFFFFFF, .reflect_input = false, .reflect_output = false, .crc_width = 32 // CRC-32/BZIP2
#define CRC_CFG_POSIX .polynomial = 0x04C11DB7, .initial_value = 0x00000000, .final_xor_value = 0xFFFFFFFF, .reflect_input = false, .reflect_output = false, .crc_width = 32       // CRC-32/POSIX
#define CRC_CFG_XFER .polynomial = 0x000000AF, .initial_value = 0x00000000, .final_xor_value = 0x00000000, .reflect_input = false, .reflect_output = false, .crc_width = 32        // CRC-32/XFER
#define CRC_CFG_JAMCRC .polynomial = 0x04C11DB7, .initial_value = 0xFFFFFFFF, .final_xor_value = 0xFFFFFFFF, .reflect_input = true, .reflect_output = true, .crc_width = 32        // CRC-32/JAMCRC
#define CRC_CFG_EDC .polynomial = 0x000000ED, .initial_value = 0x00000000, .final_xor_value = 0x00000000, .reflect_input = false, .reflect_output = false, .crc_width = 32         // CRC-32/EDC

/**
 * @brief CRC校验结构体
 */
typedef struct
{
    uint32_t polynomial;      // 多项式
    uint32_t initial_value;   // 初始值
    uint32_t final_xor_value; // 最终异或值
    bool reflect_input;       // 输入数据反转
    bool reflect_output;      // 输出数据反转
    uint8_t crc_width;        // CRC宽度（位数）
    uint32_t *table;          // CRC查表
} crc_t;

/**
 * @brief 创建CRC校验表
 * @param crc CRC结构体指针
 * @param polynomial 多项式
 * @param initial_value 初始值
 * @param final_xor_value 最终异或值
 * @param reflect_input 输入数据反转
 * @param reflect_output 输出数据反转
 * @param crc_width CRC宽度（位数）
 */
void crc_table_create(crc_t *crc);

/**
 * @brief 计算CRC校验值
 * @param crc CRC结构体指针
 * @param data 数据指针
 * @param length 数据长度
 * @return CRC校验值
 */
uint32_t crc_calculate(crc_t *crc, const uint8_t *data, uint16_t length);

/**
 * @brief 更新CRC校验值
 *
 * @param crc CRC结构体指针
 * @param last_crc_value 上一次的CRC校验值
 * @param data 数据指针
 * @param length 数据长度
 * @return uint32_t CRC校验值
 */
uint32_t crc_update(crc_t *crc, uint32_t last_crc_value, const uint8_t *data, uint16_t length);

/**
 * @brief 打印CRC校验表
 *
 * @param crc CRC结构体指针
 */
void crc_table_print(crc_t *crc);

/**
 * @file md5.h
 * @author WittXie
 * @brief 通用MD5哈希模块
 * @version 0.1
 * @date 2024-08-29
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

/**
 * @brief MD5上下文结构体
 */
typedef struct
{
    uint32_t state[4];  // 状态（ABCD）
    uint32_t count[2];  // 位计数器
    uint8_t buffer[64]; // 输入缓冲区
} md5_ctx_t;

/**
 * @brief 初始化MD5上下文
 * @param ctx MD5上下文指针
 */
void md5_init(md5_ctx_t *ctx);

/**
 * @brief 更新MD5哈希计算
 * @param ctx MD5上下文指针
 * @param input 输入数据指针
 * @param input_len 输入数据长度
 */
void md5_update(md5_ctx_t *ctx, const uint8_t *input, uint32_t input_len);

/**
 * @brief 完成MD5哈希计算并输出结果
 * @param ctx MD5上下文指针
 * @param output 输出结果缓冲区（16字节）
 */
void md5_final(md5_ctx_t *ctx, uint8_t output[16]);

/**
 * @brief 计算数据的MD5哈希值
 * @param data 数据指针
 * @param length 数据长度
 * @param output 输出结果缓冲区（16字节）
 */
void md5_calculate(const uint8_t *data, uint32_t length, uint8_t output[16]);

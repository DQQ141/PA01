/**
 * @file rsa.h
 * @author WittXie
 * @brief RSA非对称加密模块
 * @version 0.1
 * @date 2024-08-22
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

// RSA密钥结构体
typedef struct
{
    uint64_t n; // 模数
    uint64_t e; // 公钥指数
    uint64_t d; // 私钥指数
} rsa_key_t;

/**
 * @brief 初始化RSA密钥
 * @param key 密钥结构体指针
 * @param n 模数
 * @param e 公钥指数
 * @param d 私钥指数
 */
void rsa_init(rsa_key_t *key, uint64_t n, uint64_t e, uint64_t d);

/**
 * @brief RSA加密
 * @param key 密钥结构体指针
 * @param plaintext 明文
 * @param ciphertext 密文缓冲区
 * @param length 明文长度
 */
void rsa_encrypt(const rsa_key_t *key, uint8_t *ciphertext, const uint8_t *plaintext, uint32_t length);

/**
 * @brief RSA加密更新接口
 *
 * @param key 密钥结构体指针
 * @param ciphertext 密文
 * @param plaintext 明文
 * @param length 明文长度
 */
void rsa_encrypt_update(const rsa_key_t *key, uint8_t *ciphertext, const uint8_t *plaintext, uint32_t length);

/**
 * @brief RSA解密
 * @param key 密钥结构体指针
 * @param ciphertext 密文
 * @param decryptedtext 待解密的明文缓冲区
 * @param length 密文长度
 */
void rsa_decrypt(const rsa_key_t *key, const uint8_t *ciphertext, uint8_t *decryptedtext, uint32_t length);

/**
 * @brief RSA解密更新接口
 *
 * @param key 密钥结构体指针
 * @param ciphertext 密文
 * @param decryptedtext 待解密的明文缓冲区
 * @param length 密文长度
 */
void rsa_decrypt_update(const rsa_key_t *key, const uint8_t *ciphertext, uint8_t *decryptedtext, uint32_t length);

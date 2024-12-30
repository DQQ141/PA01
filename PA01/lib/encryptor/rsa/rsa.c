#include "./rsa.h"
#include <math.h>
#include <stdlib.h>

// 初始化RSA密钥
void rsa_init(rsa_key_t *key, uint64_t n, uint64_t e, uint64_t d)
{
    ASSERT(key != NULL);
    key->n = n;
    key->e = e;
    key->d = d;
}

// 快速幂模运算
static uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod)
{
    uint64_t result = 1;
    base = base % mod;
    while (exp > 0)
    {
        if (exp % 2 == 1)
        {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

// RSA加密
void rsa_encrypt(const rsa_key_t *key, uint8_t *ciphertext, const uint8_t *plaintext, uint32_t length)
{
    ASSERT(key != NULL);
    ASSERT(plaintext != NULL);
    ASSERT(ciphertext != NULL);

    for (uint32_t i = 0; i < length; i++)
    {
        ciphertext[i] = (uint8_t)mod_exp(plaintext[i], key->e, key->n);
    }
}

// RSA解密
void rsa_decrypt(const rsa_key_t *key, const uint8_t *ciphertext, uint8_t *decryptedtext, uint32_t length)
{
    ASSERT(key != NULL);
    ASSERT(ciphertext != NULL);
    ASSERT(decryptedtext != NULL);

    for (uint32_t i = 0; i < length; i++)
    {
        decryptedtext[i] = (uint8_t)mod_exp(ciphertext[i], key->d, key->n);
    }
}

// RSA加密更新接口
void rsa_encrypt_update(const rsa_key_t *key, uint8_t *ciphertext, const uint8_t *plaintext, uint32_t length)
{
    rsa_encrypt(key, ciphertext, plaintext, length);
}

// RSA解密更新接口
void rsa_decrypt_update(const rsa_key_t *key, const uint8_t *ciphertext, uint8_t *decryptedtext, uint32_t length)
{
    rsa_decrypt(key, ciphertext, decryptedtext, length);
}

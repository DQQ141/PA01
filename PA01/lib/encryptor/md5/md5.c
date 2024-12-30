#include "./md5.h"
#include <string.h>

// MD5常量
static const uint32_t MD5_S[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

static const uint32_t MD5_K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

// 填充数据
static const uint8_t padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// 辅助函数：循环左移
static uint32_t left_rotate(uint32_t x, uint32_t c)
{
    return (x << c) | (x >> (32 - c));
}

// 初始化MD5上下文
void md5_init(md5_ctx_t *ctx)
{
    ASSERT(ctx != NULL);
    ctx->count[0] = ctx->count[1] = 0;
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;
}

// 辅助函数：编码函数
static void encode(uint8_t *output, const uint32_t *input, uint32_t len)
{
    for (uint32_t i = 0, j = 0; j < len; i++, j += 4)
    {
        output[j] = (uint8_t)(input[i] & 0xff);
        output[j + 1] = (uint8_t)((input[i] >> 8) & 0xff);
        output[j + 2] = (uint8_t)((input[i] >> 16) & 0xff);
        output[j + 3] = (uint8_t)((input[i] >> 24) & 0xff);
    }
}

// 辅助函数：解码函数
static void decode(uint32_t *output, const uint8_t *input, uint32_t len)
{
    for (uint32_t i = 0, j = 0; j < len; i++, j += 4)
    {
        output[i] = ((uint32_t)input[j]) | (((uint32_t)input[j + 1]) << 8) |
                    (((uint32_t)input[j + 2]) << 16) | (((uint32_t)input[j + 3]) << 24);
    }
}

// 辅助函数：转换函数
static void md5_transform(uint32_t state[4], const uint8_t block[64])
{
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];
    decode(x, block, 64);

    // 主循环
    for (uint32_t i = 0; i < 64; i++)
    {
        uint32_t f, g;
        if (i < 16)
        {
            f = (b & c) | ((~b) & d);
            g = i;
        }
        else if (i < 32)
        {
            f = (d & b) | ((~d) & c);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48)
        {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        }
        else
        {
            f = c ^ (b | (~d));
            g = (7 * i) % 16;
        }
        uint32_t temp = d;
        d = c;
        c = b;
        b = b + left_rotate((a + f + MD5_K[i] + x[g]), MD5_S[i]);
        a = temp;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

// 更新MD5哈希计算
void md5_update(md5_ctx_t *ctx, const uint8_t *input, uint32_t input_len)
{
    ASSERT(ctx != NULL);
    ASSERT(input != NULL);

    uint32_t i, index, part_len;
    index = (uint32_t)((ctx->count[0] >> 3) & 0x3F);
    if ((ctx->count[0] += ((uint32_t)input_len << 3)) < ((uint32_t)input_len << 3))
    {
        ctx->count[1]++;
    }
    ctx->count[1] += ((uint32_t)input_len >> 29);
    part_len = 64 - index;
    if (input_len >= part_len)
    {
        memcpy(&ctx->buffer[index], input, part_len);
        md5_transform(ctx->state, ctx->buffer);
        for (i = part_len; i + 63 < input_len; i += 64)
        {
            md5_transform(ctx->state, &input[i]);
        }
        index = 0;
    }
    else
    {
        i = 0;
    }
    memcpy(&ctx->buffer[index], &input[i], input_len - i);
}

// 完成MD5哈希计算并输出结果
void md5_final(md5_ctx_t *ctx, uint8_t output[16])
{
    ASSERT(ctx != NULL);
    ASSERT(output != NULL);

    uint8_t bits[8];
    uint32_t index, pad_len;
    encode(bits, ctx->count, 8);
    index = (uint32_t)((ctx->count[0] >> 3) & 0x3f);
    pad_len = (index < 56) ? (56 - index) : (120 - index);
    md5_update(ctx, padding, pad_len);
    md5_update(ctx, bits, 8);
    encode(output, ctx->state, 16);
}

// 计算数据的MD5哈希值
void md5_calculate(const uint8_t *data, uint32_t length, uint8_t output[16])
{
    md5_ctx_t ctx;
    md5_init(&ctx);
    md5_update(&ctx, data, length);
    md5_final(&ctx, output);
}

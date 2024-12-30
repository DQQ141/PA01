/**
 * @file compare.h
 * @author WittXie
 * @brief 比较算法
 * @version 0.1
 * @date 2024-10-25
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

// 常见比较函数
#ifndef CMP_MAX
#define CMP_MAX(_a, _b) ((_a) > (_b) ? (_a) : (_b))
#endif

#ifndef CMP_MIN
#define CMP_MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))
#endif

#ifndef ABS
#define ABS(_x) ((_x) < 0 ? -(_x) : (_x))
#endif

/**
 * @brief 比较
 *
 * @param _a 参数1
 * @param _b 参数2
 * @return 0:相等，1:_a>b，-1:_a<_b
 */
#ifndef CMP
#define CMP(_a, _b) (((_a) > (_b)) ? 1 : (((_a) < (_b)) ? -1 : 0))
#endif

/**
 * @brief 字符串比较
 *
 * @param _a 字符串1
 * @param _b 字符串2
 * @return 0:相等，1:_a>b，-1:_a<_b
 */
#ifndef CMP_STR
#define CMP_STR(_a, _b) (strcmp((_a), (_b)))
#endif

/**
 * @brief 字典比较 （字典序）
 *
 * @param _a 字典1
 * @param _b 字典2
 * @return 0:相等，1:_a>b，-1:_a<_b
 */
#ifndef CMP_DICT
#define CMP_DICT(_a, _b) (memcmp(&_a, &_b, sizeof(_a)))
#endif

/**
 * @brief 浮点数比较
 *
 * @param _a 浮点数1
 * @param _b 浮点数2
 * @param _epsilon 误差
 * @return 0:相等，1:_a>b，-1:_a<_b
 */
#ifndef CMP_FLOAT
#define CMP_FLOAT(_a, _b, _epsilon) (fabs((_a) - (_b)) < (_epsilon) ? 0 : ((_a) > (_b) ? 1 : -1))
#endif

/**
 * @brief 与上一次比较
 *
 * @param _x 当前值，可以是结构体，也可以是任意类型的变量
 * @return 0:无变化，1:变化
 */
#ifndef CMP_PREV
#define CMP_PREV(_x)                      \
    ({                                    \
        static typeof(_x) _last = {0};    \
        int8_t ret = CMP_DICT(_x, _last); \
        if (ret != 0)                     \
        {                                 \
            _last = _x;                   \
        }                                 \
        ret;                              \
    })
#endif

/**
 * @brief 与上一次比较
 *
 * @param _x 当前值，可以是结构体，也可以是任意类型的变量
 * @return 0:无变化，1:变化
 */
#ifndef CMP_PREV_BIT
#define CMP_PREV_BIT(_x)          \
    ({                            \
        static int8_t _last = 0;  \
        int8_t ret = _last != _x; \
        if (ret)                  \
        {                         \
            _last = _x;           \
        }                         \
        ret;                      \
    })
#endif

/**
 * @brief 历史最小值
 *
 * @param _x 当前值，可以是结构体，也可以是任意类型的变量
 * @return 历史最小值
 */
#ifndef CMP_PREV_MIN
#define CMP_PREV_MIN(_x)                  \
    ({                                    \
        static typeof(_x) _last = {0};    \
        int8_t ret = CMP_DICT(_x, _last); \
        if (ret < 0)                      \
        {                                 \
            _last = _x;                   \
        }                                 \
        _last;                            \
    })
#endif

/**
 * @brief 历史最大值
 *
 * @param _x 当前值，可以是结构体，也可以是任意类型的变量
 * @return 历史最大值
 */
#ifndef CMP_PREV_MAX
#define CMP_PREV_MAX(_x)                  \
    ({                                    \
        static typeof(_x) _last = {0};    \
        int8_t ret = CMP_DICT(_x, _last); \
        if (ret > 0)                      \
        {                                 \
            _last = _x;                   \
        }                                 \
        _last;                            \
    })
#endif

/**
 * @brief 与上一次比较
 *
 * @param _x 当前值，只能是float类型
 * @param _epsilon 误差
 * @return 0:相等，1:_a>b，-1:_a<_b
 */
#ifndef CMP_PREV_FLOAT
#define CMP_PREV_FLOAT(_x, _epsilon)                 \
    ({                                               \
        static typeof(_x) _last = {0};               \
        int8_t ret = CMP_FLOAT(_x, _last, _epsilon); \
        if (ret != 0)                                \
        {                                            \
            _last = _x;                              \
        }                                            \
        ret;                                         \
    })
#endif

/**
 * @brief 历史最小值
 *
 * @param _x 当前值，只能是float类型
 * @param _epsilon 误差
 * @return 历史最小值
 */
#ifndef CMP_PREV_MIN_FLOAT
#define CMP_PREV_MIN_FLOAT(_x, _epsilon)             \
    ({                                               \
        static typeof(_x) _last = {0};               \
        int8_t ret = CMP_FLOAT(_x, _last, _epsilon); \
        if (ret < 0)                                 \
        {                                            \
            _last = _x;                              \
        }                                            \
        _last;                                       \
    })
#endif

/**
 * @brief 历史最大值
 *
 * @param _x 当前值，只能是float类型
 * @param _epsilon 误差
 * @return 历史最大值
 */
#ifndef CMP_PREV_MAX_FLOAT
#define CMP_PREV_MAX_FLOAT(_x, _epsilon)             \
    ({                                               \
        static typeof(_x) _last = {0};               \
        int8_t ret = CMP_FLOAT(_x, _last, _epsilon); \
        if (ret > 0)                                 \
        {                                            \
            _last = _x;                              \
        }                                            \
        _last;                                       \
    })
#endif

/**
 * @brief 四舍五入
 *
 */
#define ROUND(x) ((x) >= 0 ? (int)((x) + 0.5) : (int)((x) - 0.5))

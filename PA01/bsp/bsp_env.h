/**
 * @file bsp_env.h
 * @author WittXie
 * @brief 板级支持包环境文件
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

#define PROJECT "PA01"

#define DEBUG 0 // 调试模式
#define SW_VERSION "v1.1.1_release"

#undef LOG_LEVEL
#if DEBUG == 1
#define LOG_LEVEL LOG_LEVEL_ALL
#else
#define LOG_LEVEL LOG_LEVEL_ERROR
#endif

// 选择芯片支持包
#include "../msp/msp_hal/msp.h"

// 时间戳
#define TIMESTAMP_US timestamp_us_get(&g_time_timer5)
#define TIMESTAMP_US_GET() TIMESTAMP_US
#define TIMESTAMP_US_STARTUP (g_time_timer5.timestamp_startup)
#define SLEEP_MS(_ms) os_sleep(_ms)

// 堆：动态内存
#define MALLOC(_size) os_malloc(_size)
#define FREE(_pv) os_free(_pv)

#define STACK_MALLOC(_size) MALLOC(_size)
#define STACK_FREE(_pv) FREE(_pv)

// 互斥锁
#define MUTEX_LOCK(_mutex) os_mutex_lock(_mutex, 1000)
#define MUTEX_UNLOCK(_mutex) os_mutex_unlock(_mutex)

// 临界区
#define CRITICAL_ENTER() os_critical_enter()
#define CRITICAL_EXIT() os_critical_exit()

// 配置log默认通道
#define log_trace(_format, ...)                          \
    {                                                    \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE)                \
            bsp_debug(0, trace, _format, ##__VA_ARGS__); \
    }

#define log_info(_format, ...)                          \
    {                                                   \
        if (LOG_LEVEL >= LOG_LEVEL_INFO)                \
            bsp_debug(0, info, _format, ##__VA_ARGS__); \
    }

#define log_warn(_format, ...)                          \
    {                                                   \
        if (LOG_LEVEL >= LOG_LEVEL_WARN)                \
            bsp_debug(0, warn, _format, ##__VA_ARGS__); \
    }

#define log_error(_format, ...)                          \
    {                                                    \
        if (LOG_LEVEL >= LOG_LEVEL_ERROR)                \
            bsp_debug(0, error, _format, ##__VA_ARGS__); \
    }

#define print(_format, ...)                       \
    {                                             \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE)         \
            bsp_print(0, _format, ##__VA_ARGS__); \
    }

#define dprint(_format, ...)                       \
    {                                              \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE)          \
            bsp_dprint(0, _format, ##__VA_ARGS__); \
    }

#define print_expected(_format, _expected, _result, ...)                       \
    {                                                                          \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE)                                      \
            bsp_print_expected(0, _format, _expected, _result, ##__VA_ARGS__); \
    }

#define flush()                           \
    {                                     \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE) \
            bsp_flush(0);                 \
    }

#define print_hex8(_format, _buff, _length, ...)                       \
    {                                                                  \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE)                              \
            bsp_print_hex8(0, _format, _buff, _length, ##__VA_ARGS__); \
    }

#define print_hex16(_format, _buff, _length, ...)                       \
    {                                                                   \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE)                               \
            bsp_print_hex16(0, _format, _buff, _length, ##__VA_ARGS__); \
    }

#define print_hex32(_format, _buff, _length, ...)                       \
    {                                                                   \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE)                               \
            bsp_print_hex32(0, _format, _buff, _length, ##__VA_ARGS__); \
    }

#define print_float(_format, _buff, _length, ...)                       \
    {                                                                   \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE)                               \
            bsp_print_float(0, _format, _buff, _length, ##__VA_ARGS__); \
    }

#define print_uint16(_format, _buff, _length, ...)                       \
    {                                                                    \
        if (LOG_LEVEL >= LOG_LEVEL_TRACE)                                \
            bsp_print_uint16(0, _format, _buff, _length, ##__VA_ARGS__); \
    }

#define PRINT(_format, ...)            \
    {                                  \
        print(_format, ##__VA_ARGS__); \
    }

#define PRINT_HEX(_format, _buff, _length, ...)             \
    {                                                       \
        print_hex8(_format, _buff, _length, ##__VA_ARGS__); \
    }

#define TRACE(_format, ...)                \
    {                                      \
        log_trace(_format, ##__VA_ARGS__); \
    }

#define INFO(_format, ...)                \
    {                                     \
        log_info(_format, ##__VA_ARGS__); \
    }

#define WARN(_format, ...)                \
    {                                     \
        log_warn(_format, ##__VA_ARGS__); \
    }

#define ERROR(_format, ...)                \
    {                                      \
        log_error(_format, ##__VA_ARGS__); \
    }

#define ASSERT(_bool, ...)                                  \
    {                                                       \
        if (!(_bool))                                       \
        {                                                   \
            log_error("Assertion failed: %s.\r\n", #_bool); \
            dprint(" " __VA_ARGS__);                        \
            while (DEBUG)                                   \
            {                                               \
            }                                               \
        }                                                   \
    }

// base
#include "./bsp_base.h"

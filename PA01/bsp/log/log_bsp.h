/**
 * @file log_bsp.h
 * @author WittXie
 * @brief 日志初始化
 * @version 0.1
 * @date 2024-05-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

// 环境
#include "../bsp_env.h"

// 依赖
#include "../time/time_bsp.h"
#include "./../os/os_bsp.h"

// 根据调试模式选择日志配置
#if (DEBUG)
#define LOG_BUFF_SIZE 4096
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_ALL
#endif
#else
#undef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_NONE
#define LOG_BUFF_SIZE 32
#endif

// 驱动
#include "../../lib/log/log.h"

/**
 * @brief bsp初始化
 *
 */
void log_bsp_init(void);

/**
 * @brief 日志打印
 *
 * @param ch 通道
 * @param log 日志指针
 * @param type 日志类型 trace/info/warn/error/assert
 * @param format 格式化字符串
 * @param ... 可变参数
 */
#if (DEBUG)
#define bsp_debug(_ch, _type, _format, ...) log_debug(g_log_group[_ch], _type, _format, ##__VA_ARGS__)
#define bsp_print(_ch, _format, ...) log_print(g_log_group[_ch], _format, ##__VA_ARGS__)
#define bsp_dprint(_ch, _format, ...) log_dprint(g_log_group[_ch], _format, ##__VA_ARGS__)
#define bsp_print_expected(_ch, _format, _expected, _result, ...) log_print_expected(g_log_group[_ch], _format, _expected, _result, ##__VA_ARGS__)
#define bsp_flush(_ch) log_flush(g_log_group[_ch])
#define bsp_print_hex8(_ch, _format, _buff, _length, ...) log_print_hex8(g_log_group[_ch], _format, _buff, _length, ##__VA_ARGS__)
#define bsp_print_hex16(_ch, _format, _buff, _length, ...) log_print_hex16(g_log_group[_ch], _format, _buff, _length, ##__VA_ARGS__)
#define bsp_print_hex32(_ch, _format, _buff, _length, ...) log_print_hex32(g_log_group[_ch], _format, _buff, _length, ##__VA_ARGS__)
#define bsp_print_float(_ch, _format, _buff, _length, ...) log_print_float(g_log_group[_ch], _format, _buff, _length, ##__VA_ARGS__)
#define bsp_print_uint16(_ch, _format, _buff, _length, ...) log_print_uint16(g_log_group[_ch], _format, _buff, _length, ##__VA_ARGS__)
#else
#define bsp_debug(_ch, _type, _format, ...) ((void)0)
#define bsp_print(_ch, _format, ...) ((void)0)
#define bsp_dprint(_ch, _format, ...) ((void)0)
#define bsp_print_expected(_ch, _format, _expected, _result, ...) ((void)0)
#define bsp_flush(_ch) ((void)0)
#define bsp_print_hex8(_ch, _format, _buff, _length, ...) ((void)0)
#define bsp_print_hex16(_ch, _format, _buff, _length, ...) ((void)0)
#define bsp_print_hex32(_ch, _format, _buff, _length, ...) ((void)0)
#define bsp_print_float(_ch, _format, _buff, _length, ...) ((void)0)
#define bsp_print_uint16(_ch, _format, _buff, _length, ...) ((void)0)
#endif

// 日志声明
extern log_t g_log_uart;

// 日志组声明
extern log_t *const g_log_group[];
extern const uint32_t LOG_GROUP_SIZE;
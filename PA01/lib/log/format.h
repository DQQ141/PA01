/**
 * @file format.h
 * @author WittXie
 * @brief 日志格式化
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FORMAT ">%04u-%02u-%02u %02u:%02u:%02u.%03u%03u[%s:%d->%s()] "

#define FORMAT_CONTENT(_data) (_data)->year, (_data)->month, (_data)->day,                         \
                              (_data)->hour, (_data)->min, (_data)->sec, (_data)->ms, (_data)->us, \
                              path_remove(__FILE__), __LINE__, __func__

/**
 * @brief 打印16进制
 *
 * @param buff 数据
 * @param length 数据长度
 * @param _str_pre 开头字符串
 * @param _format 格式化字符串
 * @param _str_end 结尾字符串
 * @param num_per_line 每行打印个数
 */
#define log_print_format_hex(_log, _format, _buff, _length, _str_pre, _type, _str_end, _num_per_line, ...) \
    {                                                                                                      \
        log_print_trace((_log), COLOR_H_WHITE _format, ##__VA_ARGS__);                                     \
        for (uint32_t _i = 0; _i < _length; _i++)                                                          \
        {                                                                                                  \
            log_print((_log), _str_pre _type _str_end, (_buff)[_i]);                                       \
            if (_i % _num_per_line == _num_per_line - 1u)                                                  \
            {                                                                                              \
                log_print((_log), "\r\n");                                                                 \
            }                                                                                              \
        }                                                                                                  \
        log_print((_log), "\r\n");                                                                         \
    }

#define log_print_array(_log, _format, _type, _buff, _length, ...) log_print_format_hex(_log, _format, _buff, _length, "", _type, " ", 32, ##__VA_ARGS__)
#define log_print_hex8(_log, _format, _buff, _length, ...) log_print_array(_log, _format, "%02X", _buff, _length, ##__VA_ARGS__)
#define log_print_hex16(_log, _format, _buff, _length, ...) log_print_array(_log, _format, "%04X", _buff, _length, ##__VA_ARGS__)
#define log_print_hex32(_log, _format, _buff, _length, ...) log_print_array(_log, _format, "%08X", _buff, _length, ##__VA_ARGS__)
#define log_print_float(_log, _format, _buff, _length, ...) log_print_array(_log, _format, "%0.2f", _buff, _length, ##__VA_ARGS__)
#define log_print_uint16(_log, _format, _buff, _length, ...) log_print_array(_log, _format, "%8lu", _buff, _length, ##__VA_ARGS__)

/**
 * @brief 期望日志
 *
 * @param _log 日志指针
 * @param _message 日志消息
 * @param _expected 期望值
 * @param _result 实际值
 */
#define log_print_expected(_log, _format, _expected, _result, _length, ...)                                                                              \
    {                                                                                                                                                    \
        log_print_info(_log, _format "test begin.\r\n", ##__VA_ARGS__);                                                                                  \
        log_print(_log, COLOR_H_BLUE "expected:");                                                                                                       \
        for (uint16_t i = 0; i < _length; i++)                                                                                                           \
        {                                                                                                                                                \
            log_print(_log, "0x%02X ", _expected[i]);                                                                                                    \
        }                                                                                                                                                \
        log_flush(_log);                                                                                                                                 \
        log_print(_log, "\r\n");                                                                                                                         \
        log_print(_log, COLOR_H_CYAN "actual  :");                                                                                                       \
        for (uint16_t i = 0; i < _length; i++)                                                                                                           \
        {                                                                                                                                                \
            log_print(_log, "0x%02X ", _result[i]);                                                                                                      \
        }                                                                                                                                                \
        log_print(_log, "\r\n");                                                                                                                         \
        for (uint16_t i = 0; i < _length; i++)                                                                                                           \
        {                                                                                                                                                \
            if (_result[i] != _expected[i])                                                                                                              \
            {                                                                                                                                            \
                log_print_error(_log, _format "test failed: _expected 0x%02X, got 0x%02X at index %zu\r\n", _expected[i], _result[i], i, ##__VA_ARGS__); \
                log_flush(_log);                                                                                                                         \
                while (true)                                                                                                                             \
                {                                                                                                                                        \
                }                                                                                                                                        \
            }                                                                                                                                            \
        }                                                                                                                                                \
        log_print_info(_log, COLOR_H_GREEN _format "test pass.\r\n" COLOR_L_WHITE, ##__VA_ARGS__);                                                       \
        log_flush(_log);                                                                                                                                 \
    }

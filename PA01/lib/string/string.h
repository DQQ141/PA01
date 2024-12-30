/**
 * @file string.h
 * @author WittXie
 * @brief 字符串处理
 * @version 0.1
 * @date 2023-03-30
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

#include "./ascii.h"
#include "./hex.h"

// 配置
// 字符串操作工具
#define __DEFINE_TO_STR(R) #R
#define DEFINE_TO_STR(R) __DEFINE_TO_STR(R) // 宏定义转字符串

#define __STRCAT_2(STR1, STR2) STR1##STR2
#define STRCAT_2(STR1, STR2) __STRCAT_2(STR1, STR2) // 字符串拼接

#define __STRCAT_3(STR1, STR2, STR3) STR1##STR2##STR3
#define STRCAT_3(STR1, STR2, STR3) __STRCAT_3(STR1, STR2, STR3) // 字符串拼接

// 是否为汉字
#define is_zh(_ch) (((~(_ch >> 8) == 0) || (_ch == ' ')) ? 0 : 1)

/**
 * @brief 右移字符串
 * @param _p_str 字符串地址
 * @param _shift 移动量
 */
#define str_move_right(_p_str, _shift)                                       \
    {                                                                        \
        uint16_t _str_length = strlen(_p_str) + 1;                           \
        if ((_shift > 0) && (_str_length > 0))                               \
        {                                                                    \
            uint8_t *p_str_index = (uint8_t *)_p_str + _str_length + _shift; \
            while (p_str_index != _p_str)                                    \
            {                                                                \
                *p_str_index = *(p_str_index - _shift);                      \
                p_str_index--;                                               \
                if (p_str_index < _p_str)                                    \
                    break;                                                   \
            }                                                                \
        }                                                                    \
    }

/**
 * @brief 左移字符串
 * @param _p_str 字符串地址
 * @param _shift 移动量
 */
#define str_move_left(_p_str, _shift)                                       \
    {                                                                       \
        if (_shift > 0)                                                     \
        {                                                                   \
            memcpy((uint8_t *)_p_str - _shift, _p_str, strlen(_p_str) + 1); \
        }                                                                   \
    }

/**
 * @brief 按位置插入字符
 * @param _p_str 字符串地址
 * @param _posi 要删除的开始位置
 * @param _ch 字符
 */
#define str_insert(_p_str, _posi, _ch)                \
    {                                                 \
        str_move_right((uint8_t *)_p_str + _posi, 1); \
        *((uint8_t *)_p_str + _posi) = _ch;           \
    }

/**
 * @brief 按位置删除指定长度的字符串
 * @param _p_str 字符串地址
 * @param _posi 要删除的开始位置
 * @param _length 要删除的长度
 */
#define str_delete(_p_str, _posi, _length)                     \
    {                                                          \
        if ((_posi > 0) && (_length > 0))                      \
            str_move_left((uint8_t *)_p_str + _posi, _length); \
    }

/**
 * @brief 移除路径
 *
 * @param p_str 字符串地址
 * @return char * 移除路径后的字符串地址
 */
char *path_remove(char *p_str);

/**
 * @brief  删除行首、行尾空白符号
 *
 * @param p_str
 * @return char*
 */
char *strtrim(char *p_str);

/**
 * @brief 汉字提取
 *
 * @param buff 存储提取的汉字的缓存地址
 * @param p_str 要处理的字符串地址
 */
void chinese_extract(char *buff, char *p_str);

/**
 * @brief 字符串分拆
 *
 * @param str 字符串
 * @param delimiter 分隔符
 * @param argv 结果存储缓存
 * @param argc 结果个数
 */
void split_string(char *str, char delimiter, char *argv[], uint32_t *argc);

/**
 * 将字符串分割为参数列表，并存储到argv中，同时更新argc。
 * @param buff 存储参数字符串的缓冲区
 * @param str 待处理的字符串
 * @param delimiters 参数的分隔符字符串
 * @param ignores 要忽略的字符字符串
 * @param stop 停止字符串
 * @param argv 参数字符串的数组
 * @param argc 参数个数的指针
 */
void split_string_to_buff(char *buff, const char *str, const char *delimiters, const char *ignores, const char *stop, char *argv[], uint32_t *argc);

/**
 * @brief 字符串查找函数（指定长度）
 *
 * @param haystack 字符串
 * @param needle 查找字符串
 * @param hay_len 字符串长度
 * @param needle_len 查找的字符串长度
 * @return char* 字符串地址
 */
char *strnstr(const char *haystack, const char *needle, uint32_t hay_len, uint32_t needle_len);

/**
 * @brief 计算两个字符串相同部分的长度
 *
 * @param str1 字符串1
 * @param str2 字符串2
 * @return uint32_t 相同部分的长度
 */
uint32_t strlen_common(const char *str1, const char *str2);

/**
 * @file ascii.h
 * @author WittXie
 * @brief ASCII编码
 * @version 0.1
 * @date 2023-05-16
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

// ASCII COLOR
#define COLOR_L_BLACK "\033[0m\033[0;40;30m"
#define COLOR_L_RED "\033[0m\033[0;40;31m"
#define COLOR_L_GREEN "\033[0m\033[0;40;32m"
#define COLOR_L_YELLOW "\033[0m\033[0;40;33m"
#define COLOR_L_BLUE "\033[0m\033[0;40;34m"
#define COLOR_L_MAGENTA "\033[0m\033[0;40;35m"
#define COLOR_L_CYAN "\033[0m\033[0;40;36m"
#define COLOR_L_WHITE "\033[0m\033[0;40;37m"

#define COLOR_H_BLACK "\033[0m\033[1;40;30m"
#define COLOR_H_RED "\033[0m\033[1;40;31m"
#define COLOR_H_GREEN "\033[0m\033[1;40;32m"
#define COLOR_H_YELLOW "\033[0m\033[1;40;33m"
#define COLOR_H_BLUE "\033[0m\033[1;40;34m"
#define COLOR_H_MAGENTA "\033[0m\033[1;40;35m"
#define COLOR_H_CYAN "\033[0m\033[1;40;36m"
#define COLOR_H_WHITE "\033[0m\033[1;40;37m"

// ASCII CTRL
#define ASCII_ESC "\x1B"            // ESC
#define ASCII_EXEC ASCII_ESC "\x5B" // ASCII_ESC
#define ASCII_EXEC_AND "\x3B"       // 连接符

#define ASCII_CLEAR ASCII_EXEC "\x32\x4A"                                                // 清屏
#define ASCII_CLEAR_TAIL ASCII_EXEC "\x4B"                                               // 清行尾
#define ASCII_CLEAR_LINE ASCII_EXEC "\x32\x4B"                                           // 清除行
#define ASCII_CURSOR_GOTO(_line, _column) ASCII_EXEC _line ASCII_EXEC_AND _column "\x48" // 光标跳转

#define MOVE_UP(_n) "\033[" _n "A"
#define MOVE_DOWN(_n) "\033[" _n "B"
#define MOVE_LEFT(_n) "\033[" _n "D"
#define MOVE_RIGHT(_n) "\033[" _n "C"

// ASCII KEY
enum __ascii_key
{
    KEY_UP = 0x1B5B41,        // 光标上移
    KEY_DOWN = 0x1B5B42,      // 光标下移
    KEY_RIGHT = 0x1B5B43,     // 光标右移
    KEY_LEFT = 0x1B5B44,      // 光标左移
    KEY_HOME = 0x1B5B317E,    // 光标移动到行首
    KEY_END = 0x1B5B347E,     // 光标移动到行尾
    KEY_INSERT = 0x1B5B327E,  // 插入字符
    KEY_DELETE = 0x1B5B337E,  // 删除字符
    KEY_PAGEUP = 0x1B5B357E,  // 向上翻页
    KEY_PAGEDOWN = 0x1B5B367E // 向下翻页
};

// CTRL+KEY(小写)
#define CTRL_KEY(c) ((c) & 0x1F) // 控制键

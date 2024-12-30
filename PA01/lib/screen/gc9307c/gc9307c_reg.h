/**
 * @file gc9307c_reg.h
 * @author WittXie
 * @brief GC9307C寄存器
 * @version 0.1
 * @date 2024-09-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "./gc9307c.h"

// 显示方向宏定义
#define GC9307C_MADCTL_MY 0x80
#define GC9307C_MADCTL_MX 0x40
#define GC9307C_MADCTL_MV 0x20
#define GC9307C_MADCTL_ML 0x10
#define GC9307C_MADCTL_BGR 0x08
#define GC9307C_MADCTL_MH 0x04

// 显示方向模式宏定义
typedef enum __gc9307c_dir
{
    // 垂直显示
    GC9307C_DIR_VERTICAL = (GC9307C_MADCTL_MX | GC9307C_MADCTL_BGR),
    GC9307C_DIR_VERTICAL_FLIP_X = (GC9307C_MADCTL_MX | GC9307C_MADCTL_MH | GC9307C_MADCTL_BGR),
    GC9307C_DIR_VERTICAL_FLIP_Y = (GC9307C_MADCTL_MX | GC9307C_MADCTL_MY | GC9307C_MADCTL_BGR),
    GC9307C_DIR_VERTICAL_FLIP_XY = (GC9307C_MADCTL_MX | GC9307C_MADCTL_MY | GC9307C_MADCTL_MH | GC9307C_MADCTL_BGR),

    // 水平显示
    GC9307C_DIR_HORIZONTAL = (GC9307C_MADCTL_MV | GC9307C_MADCTL_MX | GC9307C_MADCTL_BGR),
    GC9307C_DIR_HORIZONTAL_FLIP_X = (GC9307C_MADCTL_MV | GC9307C_MADCTL_MX | GC9307C_MADCTL_MH | GC9307C_MADCTL_BGR),
    GC9307C_DIR_HORIZONTAL_FLIP_Y = (GC9307C_MADCTL_MV | GC9307C_MADCTL_MX | GC9307C_MADCTL_MY | GC9307C_MADCTL_BGR),
    GC9307C_DIR_HORIZONTAL_FLIP_XY = (GC9307C_MADCTL_MV | GC9307C_MADCTL_MX | GC9307C_MADCTL_MY | GC9307C_MADCTL_MH | GC9307C_MADCTL_BGR),
} gc9307c_dir_t;

// 像素格式宏定义
#define GC9307C_COLMOD_16BIT 0x05
#define GC9307C_COLMOD_18BIT 0x06

// 常用指令的宏定义
typedef enum __gc9307c_cmd
{
    GC9307C_CMD_NOP = 0x00,      // 无操作命令，用于保持通信
    GC9307C_CMD_SWRESET = 0x01,  // 软件复位命令，用于复位显示控制器
    GC9307C_CMD_RDDID = 0x04,    // 读取设备ID命令，用于获取显示控制器的ID
    GC9307C_CMD_RDDST = 0x09,    // 读取状态命令，用于获取显示控制器的状态信息
    GC9307C_CMD_SLPIN = 0x10,    // 进入睡眠模式命令，用于将显示控制器置于低功耗状态
    GC9307C_CMD_SLPOUT = 0x11,   // 退出睡眠模式命令，用于唤醒显示控制器
    GC9307C_CMD_PTLON = 0x12,    // 部分显示区域开启命令，用于开启部分显示区域
    GC9307C_CMD_NORON = 0x13,    // 正常显示区域开启命令，用于开启整个显示区域
    GC9307C_CMD_INVOFF = 0x20,   // 关闭反转显示命令，用于关闭显示颜色的反转
    GC9307C_CMD_INVON = 0x21,    // 开启反转显示命令，用于开启显示颜色的反转
    GC9307C_CMD_DISPOFF = 0x28,  // 关闭显示命令，用于关闭显示输出
    GC9307C_CMD_DISPON = 0x29,   // 开启显示命令，用于开启显示输出
    GC9307C_CMD_CASET = 0x2A,    // 列地址设置命令，用于设置显示区域的列地址范围
    GC9307C_CMD_PASET = 0x2B,    // 页地址设置命令，用于设置显示区域的页地址范围
    GC9307C_CMD_RAMWR = 0x2C,    // 写入显存命令，用于向显存写入数据
    GC9307C_CMD_RAMRD = 0x2E,    // 读取显存命令，用于从显存读取数据
    GC9307C_CMD_PTLAR = 0x30,    // 部分显示区域设置命令，用于设置部分显示区域的地址范围
    GC9307C_CMD_VSCRDEF = 0x33,  // 垂直滚动定义命令，用于定义垂直滚动区域
    GC9307C_CMD_TEOFF = 0x34,    // 关闭TE（ tearing effect）信号命令，用于关闭TE信号输出
    GC9307C_CMD_TEON = 0x35,     // 开启TE（ tearing effect）信号命令，用于开启TE信号输出
    GC9307C_CMD_MADCTL = 0x36,   // 内存访问控制命令，用于设置内存访问的方向和镜像
    GC9307C_CMD_VSCRSADD = 0x37, // 垂直滚动地址设置命令，用于设置垂直滚动的起始地址
    GC9307C_CMD_IDLEOFF = 0x38,  // 关闭空闲模式命令，用于关闭空闲模式
    GC9307C_CMD_IDLEON = 0x39,   // 开启空闲模式命令，用于开启空闲模式
    GC9307C_CMD_COLMOD = 0x3A,   // 颜色模式设置命令，用于设置显示的颜色格式
} gc9307c_cmd_t;

// 颜色定义
#define GC9307C_COLOR_BLACK 0x0000
#define GC9307C_COLOR_WHITE 0xFFFF
#define GC9307C_COLOR_RED 0x00F8
#define GC9307C_COLOR_GREEN 0xE007
#define GC9307C_COLOR_BLUE 0x1F00
#define GC9307C_COLOR_YELLOW 0xE0FF
#define GC9307C_COLOR_CYAN 0xFF07
#define GC9307C_COLOR_MAGENTA 0x1FF8
#define GC9307C_COLOR_ORANGE 0x20FD
#define GC9307C_COLOR_PURPLE 0x0F78

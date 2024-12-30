/**
 * @file fs_stick_protocol.h
 * @author WittXie
 * @brief 富斯摇杆协议
 * @version 0.1
 * @date 2024-11-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MAP_TO_RANGE(_min_val, _mid_val, _max_val, _input)                                 \
    ((_input) < (_min_val) ? (_min_val) : ((_input) > (_max_val) ? (_max_val) : (_input)), \
     (2.0f * ((_input) - (_min_val)) / ((_max_val) - (_min_val))) - 1.0f)

// 摇杆霍尔值
typedef struct __fs_protocol_stick_hall
{
    int16_t channel[4]; // 摇杆通道
    union
    {
        uint8_t value;
        struct
        {
            uint8_t stick_1 : 4; // 摇杆1
            uint8_t stick_2 : 4; // 摇杆2
        };
    } status; // 摇杆通道状态
} fs_protocol_stick_hall_t;
#define FS_PROTOCOL_STICK_HALL_CMD 0xC4
#define FS_PROTOCOL_STICK_HALL_DATA_LENGTH (offsetof(fs_protocol_stick_hall_t, status) + sizeof(((fs_protocol_stick_hall_t *)0)->status)) // 兼容老代码，4字节对齐，这里不能直接用sizeof
#define FS_PROTOCOL_STICK_HALL_DATA_SET(_hall, _array)             \
    {                                                              \
        memcpy(_hall, _array, FS_PROTOCOL_STICK_HALL_DATA_LENGTH); \
    }

// 摇杆版本号
typedef struct __fs_protocol_stick_version
{
    uint8_t unique_id[12]; // 唯一ID
    uint8_t software[4];   // 软件版本
    uint8_t hardware[4];   // 硬件版本
} fs_protocol_stick_version_t;
#define FS_PROTOCOL_STICK_VERSION_CMD 0xB4
#define FS_PROTOCOL_STICK_VERSION_DATA_LENGTH (offsetof(fs_protocol_stick_version_t, hardware) + sizeof(((fs_protocol_stick_version_t *)0)->hardware)) // 兼容老代码，4字节对齐，这里不能直接用sizeof
#define FS_PROTOCOL_STICK_VERSION_DATA_SET(_version, _array)             \
    {                                                                    \
        memcpy(_version, _array, FS_PROTOCOL_STICK_VERSION_DATA_LENGTH); \
    }

// 摇杆参数
typedef struct __fs_protocol_stick_param
{
    struct
    {
        int16_t min;      // 最小值
        int16_t mid;      // 中间值
        int16_t max;      // 最大值
    } parameter[4];       // 参数
    uint8_t status_reset; // 重置状态 0~255
} fs_protocol_stick_param_t;
#define FS_PROTOCOL_STICK_PARAM_CMD 0xE4
#define FS_PROTOCOL_STICK_PARAM_DATA_LENGTH (offsetof(fs_protocol_stick_param_t, status_reset) + sizeof(((fs_protocol_stick_param_t *)0)->status_reset)) // 兼容老代码， 4字节对齐，这里不能直接用sizeof
#define FS_PROTOCOL_STICK_PARAM_DATA_SET(_param, _array)             \
    {                                                                \
        memcpy(_param, _array, FS_PROTOCOL_STICK_PARAM_DATA_LENGTH); \
    }

/**
 * @brief 处理霍尔传感器数据并映射到指定范围
 *
 * 该函数根据霍尔传感器的状态和参数，将传感器数据映射到指定的范围，并将结果存储在传入的指针所指向的变量中。
 *
 * @param _data_hall_ptr 指向包含霍尔传感器数据的结构体的指针
 * @param _x0_ptr 指向存储第一个霍尔传感器X轴映射结果的变量的指针
 * @param _y0_ptr 指向存储第一个霍尔传感器Y轴映射结果的变量的指针
 * @param _x1_ptr 指向存储第二个霍尔传感器X轴映射结果的变量的指针
 * @param _y1_ptr 指向存储第二个霍尔传感器Y轴映射结果的变量的指针
 */
#define FS_PROTOCOL_STICK_HALL_EXCHANGE(_data_hall_ptr, _data_param_ptr, _x0_ptr, _y0_ptr, _x1_ptr, _y1_ptr) \
    do                                                                                                       \
    {                                                                                                        \
        if (((_data_hall_ptr)->status.stick_1 == 1) &&                                                       \
            ((_data_param_ptr)->parameter[0].max != 0))                                                      \
        {                                                                                                    \
            *(_x0_ptr) = -MAP_TO_RANGE((_data_param_ptr)->parameter[0].min,                                  \
                                       (_data_param_ptr)->parameter[0].mid,                                  \
                                       (_data_param_ptr)->parameter[0].max,                                  \
                                       (_data_hall_ptr)->channel[0]);                                        \
            *(_y0_ptr) = MAP_TO_RANGE((_data_param_ptr)->parameter[1].min,                                   \
                                      (_data_param_ptr)->parameter[1].mid,                                   \
                                      (_data_param_ptr)->parameter[1].max,                                   \
                                      (_data_hall_ptr)->channel[1]);                                         \
        }                                                                                                    \
        else                                                                                                 \
        {                                                                                                    \
            *(_x0_ptr) = *(_y0_ptr) = 0;                                                                     \
        }                                                                                                    \
        if (((_data_hall_ptr)->status.stick_2 == 1) &&                                                       \
            ((_data_param_ptr)->parameter[2].max != 0))                                                      \
        {                                                                                                    \
            *(_y1_ptr) = MAP_TO_RANGE((_data_param_ptr)->parameter[2].min,                                   \
                                      (_data_param_ptr)->parameter[2].mid,                                   \
                                      (_data_param_ptr)->parameter[2].max,                                   \
                                      (_data_hall_ptr)->channel[2]);                                         \
            *(_x1_ptr) = -MAP_TO_RANGE((_data_param_ptr)->parameter[3].min,                                  \
                                       (_data_param_ptr)->parameter[3].mid,                                  \
                                       (_data_param_ptr)->parameter[3].max,                                  \
                                       (_data_hall_ptr)->channel[3]);                                        \
        }                                                                                                    \
        else                                                                                                 \
        {                                                                                                    \
            *(_x1_ptr) = *(_y1_ptr) = 0;                                                                     \
        }                                                                                                    \
    } while (0)

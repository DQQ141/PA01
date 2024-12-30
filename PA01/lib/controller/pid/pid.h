/**
 * @file pid.h
 * @author WittXie
 * @brief 通用PID控制模块
 * @version 0.1
 * @date 2024-08-28
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

/**
 * @brief PID控制结构体
 */
typedef struct
{
    float kp;           // 比例系数
    float ki;           // 积分系数
    float kd;           // 微分系数
    float setpoint;     // 设定值
    float last_error;   // 上一次的误差
    float integral;     // 积分项
    float output_limit; // 输出限制
} pid_t;

/**
 * @brief 初始化PID控制器
 * @param pid PID结构体指针
 * @param kp 比例系数
 * @param ki 积分系数
 * @param kd 微分系数
 * @param setpoint 设定值
 * @param output_limit 输出限制
 */
void pid_init(pid_t *pid, float kp, float ki, float kd, float setpoint, float output_limit);

/**
 * @brief 计算PID控制输出
 * @param pid PID结构体指针
 * @param feedback 反馈值
 * @return PID控制输出
 */
float pid_update(pid_t *pid, float feedback);

/**
 * @brief 重置PID控制器
 * @param pid PID结构体指针
 */
void pid_reset(pid_t *pid);

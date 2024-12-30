/**
 * @file foc.h
 * @author WittXie
 * @brief 通用FOC算法模块
 * @version 0.1
 * @date 2024-08-29
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "./../pid/pid.h"
#include <stdbool.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

/**
 * @brief FOC控制结构体
 */
typedef struct
{
    pid_t id_pid; // d轴PID控制器
    pid_t iq_pid; // q轴PID控制器
    float theta;  // 电机角度
    float speed;  // 电机速度
    float vd;     // d轴电压
    float vq;     // q轴电压
    float id;     // d轴电流
    float iq;     // q轴电流
} foc_t;

/**
 * @brief 初始化FOC控制器
 * @param foc FOC结构体指针
 * @param kp_d d轴比例增益
 * @param ki_d d轴积分增益
 * @param kd_d d轴微分增益
 * @param kp_q q轴比例增益
 * @param ki_q q轴积分增益
 * @param kd_q q轴微分增益
 * @param output_limit 输出限幅
 */
void foc_init(foc_t *foc,
              float kp_d, float ki_d, float kd_d,
              float kp_q, float ki_q, float kd_q,
              float output_limit);

/**
 * @brief 更新FOC控制器
 * @param foc FOC结构体指针
 * @param id_ref d轴电流参考值
 * @param iq_ref q轴电流参考值
 * @param id_meas d轴电流测量值
 * @param iq_meas q轴电流测量值
 */
void foc_update(foc_t *foc,
                float id_ref, float iq_ref,
                float id_meas, float iq_meas);

/**
 * @brief 设置电机角度和速度
 * @param foc FOC结构体指针
 * @param theta 电机角度
 * @param speed 电机速度
 */
void foc_set_theta_speed(foc_t *foc, float theta, float speed);

/**
 * @brief 获取d轴和q轴电压
 * @param foc FOC结构体指针
 * @param vd 返回的d轴电压
 * @param vq 返回的q轴电压
 */
void foc_get_vd_vq(foc_t *foc, float *vd, float *vq);

/**
 * @brief 释放FOC控制器内存
 * @param foc FOC结构体指针
 */
void foc_deinit(foc_t *foc);

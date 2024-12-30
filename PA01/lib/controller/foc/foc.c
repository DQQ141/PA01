#include "./foc.h"
#include <stdlib.h>

// 初始化FOC控制器
void foc_init(foc_t *foc,
              float kp_d, float ki_d, float kd_d,
              float kp_q, float ki_q, float kd_q,
              float output_limit)
{
    ASSERT(foc != NULL);

    // 初始化d轴PID控制器
    pid_init(&foc->id_pid, kp_d, ki_d, kd_d, 0, output_limit);

    // 初始化q轴PID控制器
    pid_init(&foc->iq_pid, kp_q, ki_q, kd_q, 0, output_limit);

    // 初始化其他参数
    foc->theta = 0;
    foc->speed = 0;
    foc->vd = 0;
    foc->vq = 0;
    foc->id = 0;
    foc->iq = 0;
}

// 更新FOC控制器
void foc_update(foc_t *foc,
                float id_ref, float iq_ref,
                float id_meas, float iq_meas)
{
    ASSERT(foc != NULL);

    // 更新d轴PID控制器
    foc->id_pid.setpoint = id_ref;
    foc->vd = pid_update(&foc->id_pid, id_meas);

    // 更新q轴PID控制器
    foc->iq_pid.setpoint = iq_ref;
    foc->vq = pid_update(&foc->iq_pid, iq_meas);

    // 更新d轴和q轴电流
    foc->id = id_meas;
    foc->iq = iq_meas;
}

// 设置电机角度和速度
void foc_set_theta_speed(foc_t *foc, float theta, float speed)
{
    ASSERT(foc != NULL);
    foc->theta = theta;
    foc->speed = speed;
}

// 获取d轴和q轴电压
void foc_get_vd_vq(foc_t *foc, float *vd, float *vq)
{
    ASSERT(foc != NULL);
    *vd = foc->vd;
    *vq = foc->vq;
}

// 释放FOC控制器内存
void foc_deinit(foc_t *foc)
{
    ASSERT(foc != NULL);
    // 不需要释放内存，因为所有内存都是静态分配的
}

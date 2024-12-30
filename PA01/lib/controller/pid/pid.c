#include "./pid.h"

// 初始化PID控制器
void pid_init(pid_t *pid, float kp, float ki, float kd, float setpoint, float output_limit)
{
    ASSERT(pid != NULL);

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->setpoint = setpoint;
    pid->output_limit = output_limit;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
}

// 计算PID控制输出
float pid_update(pid_t *pid, float feedback)
{
    ASSERT(pid != NULL);

    float error = pid->setpoint - feedback; // 计算误差
    pid->integral += error;                 // 更新积分项

    // 防止积分项过大
    if (pid->integral > pid->output_limit)
    {
        pid->integral = pid->output_limit;
    }
    else if (pid->integral < -pid->output_limit)
    {
        pid->integral = -pid->output_limit;
    }

    float derivative = error - pid->last_error; // 计算微分项
    pid->last_error = error;                    // 更新上一次的误差

    float output = (pid->kp * error) + (pid->ki * pid->integral) + (pid->kd * derivative); // 计算PID输出

    // 限制输出范围
    if (output > pid->output_limit)
    {
        output = pid->output_limit;
    }
    else if (output < -pid->output_limit)
    {
        output = -pid->output_limit;
    }

    return output;
}

// 重置PID控制器
void pid_reset(pid_t *pid)
{
    ASSERT(pid != NULL);

    pid->last_error = 0.0f;
    pid->integral = 0.0f;
}

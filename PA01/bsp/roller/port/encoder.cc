/**
 * @file encoder.cc
 * @author WittXie
 * @brief 编码器驱动
 * @version 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../roller_bsp.h"

static void g_roller_init(void)
{
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1 | TIM_CHANNEL_2);
    __HAL_TIM_SET_COUNTER(&htim3, 0); // 脉冲计数值清0
}
int32_t g_roller_read(void)
{
    static uint16_t last_cnt = 0;                 // 上一个计数值，初始为 0
    uint16_t cnt = __HAL_TIM_GET_COUNTER(&htim3); // 获取当前计数值

    // 计算变化量
    int32_t delta = cnt - last_cnt;

    // 处理环绕情况
    if (delta > 0x7FFF)
    {                     // 大于 32767，表示正向环绕
        delta -= 0x10000; // 减去 65536
    }
    else if (delta < -0x7FFF)
    {                     // 小于 -32768，表示反向环绕
        delta += 0x10000; // 加上 65536
    }

    last_cnt = cnt; // 更新上一个计数值
    return delta;   // 返回变化量
}
roller_t g_roller = {
    .cfg = {
        .name = "g_roller",
        .filter_us = 30000, // 30ms 滤波
        .div = 1,           // N+1 分频, 0为不分频
    },
    .ops = {
        .init = g_roller_init,
        .read = g_roller_read,
    },
};

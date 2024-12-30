#include "./bp_filter.h"

// 初始化带通滤波器
void bp_filter_init(bp_filter_t *filter, float center_freq, float bandwidth, float sample_rate)
{
    // 断言检查，确保传入的指针不为空
    ASSERT(filter != NULL);
    // 断言检查，确保中心频率、带宽和采样率大于0
    ASSERT(center_freq > 0);
    ASSERT(bandwidth > 0);
    ASSERT(sample_rate > 0);

    // 设置滤波器的中心频率、带宽和采样率
    filter->center_freq = center_freq;
    filter->bandwidth = bandwidth;
    filter->sample_rate = sample_rate;

    // 计算滤波系数 alpha 和 beta
    float omega = 2.0f * M_PI * center_freq / sample_rate;
    float gamma = cosf(omega) / (1 + sinf(omega));
    filter->alpha = (1 - gamma) / 2;
    filter->beta = -gamma;

    // 初始化上一次的输入和输出值为0
    filter->prev_input = 0.0f;
    filter->prev_output = 0.0f;
}

// 更新带通滤波器的输出值
float bp_filter(bp_filter_t *filter, float input)
{
    ASSERT(filter != NULL);

    // 计算新的输出值
    float output = filter->alpha * (input - filter->prev_input) + filter->beta * filter->prev_output;

    // 更新上一次的输入和输出值
    filter->prev_input = input;
    filter->prev_output = output;

    // 返回滤波后的输出值
    return output;
}

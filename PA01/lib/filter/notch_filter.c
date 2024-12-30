#include "./notch_filter.h"

// 初始化带阻滤波器
void notch_filter_init(notch_filter_t *filter, float center_freq, float bandwidth, float sample_rate)
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
    float tan_value = tan(M_PI * bandwidth / sample_rate);
    filter->alpha = 1.0f / (1.0f + tan_value);
    filter->beta = -2.0f * cos(2.0f * M_PI * center_freq / sample_rate);

    // 初始化上一次的输入和输出值为0
    filter->prev_input = 0.0f;
    filter->prev_output = 0.0f;
}

// 更新带阻滤波器的输出值
float notch_filter(notch_filter_t *filter, float input)
{
    ASSERT(filter != NULL);

    float output = filter->alpha * (input + filter->prev_input) + filter->beta * filter->prev_output;
    filter->prev_output = output;
    filter->prev_input = input;
    return output;
}

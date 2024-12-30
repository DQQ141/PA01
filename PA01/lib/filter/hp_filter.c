#include "./hp_filter.h"

// 初始化高通滤波器
void hp_filter_init(hp_filter_t *filter, float cutoff_freq, float sample_rate)
{
    // 断言检查，确保传入的指针不为空
    ASSERT(filter != NULL);
    // 断言检查，确保截止频率和采样率大于0
    ASSERT(cutoff_freq > 0);
    ASSERT(sample_rate > 0);

    // 设置滤波器的截止频率和采样率
    filter->cutoff_freq = cutoff_freq;
    filter->sample_rate = sample_rate;

    // 计算滤波系数 alpha
    // alpha = 1 / (1 + 2 * PI * cutoff_freq / sample_rate)
    filter->alpha = 1.0f / (1.0f + 2.0f * M_PI * cutoff_freq / sample_rate);

    // 初始化上一次的输入和输出值为0
    filter->prev_input = 0.0f;
    filter->prev_output = 0.0f;
}

// 更新高通滤波器的输出值
float hp_filter(hp_filter_t *filter, float input)
{
    ASSERT(filter != NULL);

    float output = filter->alpha * (filter->prev_output + input - filter->prev_input);
    filter->prev_output = output;
    filter->prev_input = input;
    return output;
}

#include "./lp_filter.h"
#include <math.h>

// 初始化低通滤波器
void lp_filter_init(lp_filter_t *filter, float cutoff_freq, float sample_rate, uint16_t order)
{
    // 断言检查，确保传入的指针不为空
    ASSERT(filter != NULL);
    if (filter == NULL)
    {
        // 错误处理代码
        return;
    }

    // 断言检查，确保截止频率和采样率大于0
    ASSERT(cutoff_freq > 0);
    ASSERT(sample_rate > 0);
    if (cutoff_freq <= 0 || sample_rate <= 0)
    {
        // 错误处理代码
        return;
    }

    // 断言检查，确保阶数大于0
    ASSERT(order > 0);
    if (order <= 0)
    {
        // 错误处理代码
        return;
    }

    // 设置滤波器的截止频率和采样率
    filter->cutoff_freq = cutoff_freq;
    filter->sample_rate = sample_rate;
    filter->order = order;

    // 计算滤波系数 alpha
    filter->alpha = 1.0f / (1.0 + (1.0 / (2.0 * M_PI * cutoff_freq)) * sample_rate);

    // 初始化上一次的输出值为0
    filter->prev_input = 0.0f;
    filter->prev_output = 0.0f;
    filter->is_first = true;
}

// 更新低通滤波器的输出值
__STATIC_INLINE float _lp_filter(lp_filter_t *filter, float input)
{
    filter->prev_output = filter->alpha * (filter->alpha * input + filter->prev_input * (1.0f - filter->alpha)) + (1.0f - filter->alpha) * filter->prev_output;
    filter->prev_input = input;
    return filter->prev_output;
}

float lp_filter(lp_filter_t *filter, float input)
{
    ASSERT(filter != NULL);
    if (filter == NULL)
    {
        // 错误处理代码
        return 0.0f;
    }

    if (filter->is_first)
    {
        filter->prev_output = input;
        filter->prev_input = input;
        filter->is_first = false;
        return filter->prev_output;
    }

    for (int i = 0; i < filter->order; i++)
    {
        input = _lp_filter(filter, input); // 串联
    }
    return filter->prev_output;
}

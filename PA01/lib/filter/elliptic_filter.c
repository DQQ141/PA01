#include "./elliptic_filter.h"

// 初始化椭圆滤波器
void elliptic_filter_init(elliptic_filter_t *filter,
                          elliptic_filter_coeff_t coeff,
                          float cutoff_freq,
                          float sample_rate,
                          float ripple,
                          float stopband_attenuation)
{
    // 断言检查，确保传入的指针不为空
    ASSERT(filter != NULL);
    // 断言检查，确保截止频率和采样率大于0
    ASSERT(cutoff_freq > 0);
    ASSERT(sample_rate > 0);
    ASSERT(ripple > 0);
    ASSERT(stopband_attenuation > 0);

    // 设置滤波器的参数
    filter->coeff = coeff;
    filter->cutoff_freq = cutoff_freq;
    filter->sample_rate = sample_rate;
    filter->ripple = ripple;
    filter->stopband_attenuation = stopband_attenuation;

    // 初始化上一次的输入和输出值为0
    filter->x1 = 0.0f;
    filter->x2 = 0.0f;
    filter->y1 = 0.0f;
    filter->y2 = 0.0f;
}

// 更新椭圆滤波器的输出值
float elliptic_filter(elliptic_filter_t *filter, float input)
{
    // 断言检查，确保传入的指针不为空
    ASSERT(filter != NULL);

    // 计算当前输出值
    float output = filter->coeff.b0 * input + filter->coeff.b1 * filter->x1 + filter->coeff.b2 * filter->x2 - filter->coeff.a1 * filter->y1 - filter->coeff.a2 * filter->y2;

    // 更新历史值
    filter->x2 = filter->x1;
    filter->x1 = input;
    filter->y2 = filter->y1;
    filter->y1 = output;

    return output;
}

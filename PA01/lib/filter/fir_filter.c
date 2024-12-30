#include "./fir_filter.h"
#include <stdlib.h>

// 初始化FIR滤波器
void fir_filter_init(fir_filter_t *filter, float *coeff, uint16_t coeff_length, float *buff, uint16_t buff_length)
{
    // 断言检查，确保传入的指针不为空
    ASSERT(filter != NULL);
    ASSERT(coeff != NULL);
    ASSERT(coeff_length > 0);
    ASSERT(buff_length >= coeff_length);
    ASSERT(buff != NULL);

    // 设置滤波器系数和长度
    filter->coeff = coeff;
    filter->coeff_length = coeff_length;

    // 分配输入缓冲区
    filter->buff = buff;
    filter->buff_length = buff_length;

    // 初始化缓冲区索引
    filter->idx = 0;

    // 初始化缓冲区为0
    for (uint16_t i = 0; i < buff_length; i++)
    {
        filter->buff[i] = 0.0f;
    }
}

// 更新FIR滤波器的输出值
float fir_filter(fir_filter_t *filter, float input)
{
    ASSERT(filter != NULL);

    // 更新缓冲区
    filter->buff[filter->idx] = input;

    // 计算输出值
    float output = 0.0f;
    uint16_t j = filter->idx;
    for (uint16_t i = 0; i < filter->coeff_length; i++)
    {
        output += filter->coeff[i] * filter->buff[j];
        if (j == 0)
        {
            j = filter->buff_length - 1;
        }
        else
        {
            j--;
        }
    }

    // 更新缓冲区索引
    filter->idx = (filter->idx + 1) % filter->buff_length;
    return output;
}

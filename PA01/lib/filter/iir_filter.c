#include "./iir_filter.h"
#include <stdlib.h>

// 初始化IIR滤波器
void iir_filter_init(iir_filter_t *filter,
                     float *fw_coeff,
                     uint16_t fw_length,
                     float *bk_coeff,
                     uint16_t bk_length,
                     float *input_buff,
                     float *output_buff,
                     uint16_t buff_length)
{
    // 断言检查，确保传入的指针不为空
    ASSERT(filter != NULL);
    ASSERT(fw_coeff != NULL);
    ASSERT(bk_coeff != NULL);
    ASSERT(fw_length > 0);
    ASSERT(bk_length > 0);
    ASSERT(input_buff != NULL);
    ASSERT(output_buff != NULL);
    ASSERT(buff_length >= (fw_length > bk_length ? fw_length : bk_length));

    // 设置滤波器系数和长度
    filter->fw_coeff = fw_coeff;
    filter->bk_coeff = bk_coeff;
    filter->fw_length = fw_length;
    filter->bk_length = bk_length;

    // 分配输入和输出缓冲区
    filter->input_buff = input_buff;
    filter->output_buff = output_buff;
    filter->buff_length = buff_length;

    // 初始化缓冲区索引
    filter->idx = 0;

    // 初始化缓冲区为0
    for (uint16_t i = 0; i < buff_length; i++)
    {
        filter->input_buff[i] = 0.0f;
        filter->output_buff[i] = 0.0f;
    }
}

// 更新IIR滤波器的输出值
float iir_filter(iir_filter_t *filter, float input)
{
    ASSERT(filter != NULL);

    // 更新输入缓冲区
    filter->input_buff[filter->idx] = input;

    // 计算输出值
    float output = 0.0f;
    uint16_t j = filter->idx;
    for (uint16_t i = 0; i < filter->fw_length; i++)
    {
        output += filter->fw_coeff[i] * filter->input_buff[j];
        if (j == 0)
        {
            j = filter->buff_length - 1;
        }
        else
        {
            j--;
        }
    }

    j = filter->idx;
    for (uint16_t i = 1; i < filter->bk_length; i++)
    {
        output -= filter->bk_coeff[i] * filter->output_buff[j];
        if (j == 0)
        {
            j = filter->buff_length - 1;
        }
        else
        {
            j--;
        }
    }

    // 更新输出缓冲区
    filter->output_buff[filter->idx] = output;

    // 更新缓冲区索引
    filter->idx = (filter->idx + 1) % filter->buff_length;
    return output;
}

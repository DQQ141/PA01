#include "./fft.h"

// 辅助函数：计算旋转因子
static complex_t twiddle_factor(uint16_t k, uint16_t N, bool inverse)
{
    float angle = (inverse ? 2.0 : -2.0) * M_PI * k / N;
    complex_t result = {cosf(angle), sinf(angle)};
    return result;
}

// 辅助函数：位反转重排序
static uint16_t bit_reverse(uint16_t num, uint16_t bits)
{
    uint16_t reversed = 0;
    for (uint16_t i = 0; i < bits; i++)
    {
        reversed <<= 1;
        reversed |= (num & 1);
        num >>= 1;
    }
    return reversed;
}

// 初始化FFT
void fft_init(fft_t *fft, uint16_t size, bool inverse, complex_t *input_buffer, complex_t *output_buffer)
{
    ASSERT(fft != NULL);
    ASSERT(size > 0);
    ASSERT(input_buffer != NULL);
    ASSERT(output_buffer != NULL);

    fft->size = size;
    fft->inverse = inverse;
    fft->input = input_buffer;
    fft->output = output_buffer;
}

// 执行FFT计算
void fft_execute(fft_t *fft)
{
    ASSERT(fft != NULL);

    uint16_t N = fft->size;
    uint16_t bits = 0;
    while ((N >> bits) > 0)
    {
        bits++;
    }
    bits--;

    // 位反转重排序
    for (uint16_t i = 0; i < N; i++)
    {
        uint16_t j = bit_reverse(i, bits);
        if (j > i)
        {
            fft->output[j] = fft->input[i];
            fft->output[i] = fft->input[j];
        }
        else
        {
            fft->output[i] = fft->input[i];
        }
    }

    // 迭代FFT计算
    for (uint16_t s = 1; s <= bits; s++)
    {
        uint16_t m = 1 << s;
        uint16_t m2 = m >> 1;
        complex_t w = {1.0, 0.0};
        complex_t wm = twiddle_factor(1, m, fft->inverse);

        for (uint16_t j = 0; j < m2; j++)
        {
            for (uint16_t k = j; k < N; k += m)
            {
                uint16_t t1 = k + m2;
                complex_t t = {w.real * fft->output[t1].real - w.imag * fft->output[t1].imag,
                               w.real * fft->output[t1].imag + w.imag * fft->output[t1].real};
                complex_t u = fft->output[k];
                fft->output[k].real = u.real + t.real;
                fft->output[k].imag = u.imag + t.imag;
                fft->output[t1].real = u.real - t.real;
                fft->output[t1].imag = u.imag - t.imag;
            }
            complex_t temp = {w.real * wm.real - w.imag * wm.imag,
                              w.real * wm.imag + w.imag * wm.real};
            w = temp;
        }
    }

    // 如果是逆FFT，结果需要除以N
    if (fft->inverse)
    {
        for (uint16_t i = 0; i < N; i++)
        {
            fft->output[i].real /= N;
            fft->output[i].imag /= N;
        }
    }
}

#include "./gpio.h"

// 论调脉冲
void pulse_poll(gpio_t *gpio)
{
    ASSERT(gpio != NULL);

    if (gpio->pulse.valid_width + gpio->pulse.invalid_width == 0)
    {
        return;
    }

    uint64_t timestamp_us = TIMESTAMP_US_GET();
    uint64_t period = gpio->pulse.valid_width + gpio->pulse.invalid_width;

    if ((timestamp_us - gpio->pulse.start_point) % period < gpio->pulse.valid_width)
    {
        // 有效电平
        if (gpio->ops.read() == 0)
        {
            gpio->ops.write(gpio->cfg.value);
        }
    }
    else
    {
        // 无效电平
        if (gpio->ops.read() != 0)
        {
            gpio->ops.write(0);
        }
    }

    if (gpio->pulse.count != 0)
    {
        uint64_t remain = 0;
        if (gpio->pulse.end_point < timestamp_us)
        {
            remain = 0;
        }
        else
        {
            remain = (gpio->pulse.end_point - timestamp_us + period / 2) / period; // 计算剩余次数，四舍五入
        }

        if (gpio->pulse.count != remain)
        {
            gpio->pulse.count = remain;
            if (gpio->pulse.count == 0)
            {
                gpio_pulse_period_write(gpio, 0, 0);
                dds_publish(gpio, &(gpio->PULSE_FINISHED), NULL);
            }
        }
    }
}

// 写频率和占空比
void gpio_pulse_freq_write(gpio_t *gpio, float freq, float duty)
{
    ASSERT(gpio != NULL);

    if (gpio->flag.is_inited == false)
    {
        ERROR("[%s] gpio_pulse_freq_write failed, gpio is not inited", gpio->cfg.name);
        return;
    }

    if (freq == 0 || duty == 0)
    {
        gpio->ops.write(0);
        gpio->pulse.valid_width = 0;
        gpio->pulse.invalid_width = 0;
    }
    gpio->pulse.start_point = TIMESTAMP_US_GET();

    uint32_t period = 1000000 / freq;
    gpio->pulse.valid_width = period * duty;
    gpio->pulse.invalid_width = period - gpio->pulse.valid_width;
}

// 写宽度
void gpio_pulse_width_write(gpio_t *gpio, uint32_t valid_width, uint32_t invalid_width)
{
    ASSERT(gpio != NULL);

    if (gpio->flag.is_inited == false)
    {
        ERROR("[%s] gpio_pulse_width_write failed, gpio is not inited", gpio->cfg.name);
        return;
    }

    gpio->pulse.start_point = TIMESTAMP_US_GET();
    gpio->pulse.valid_width = valid_width;
    gpio->pulse.invalid_width = invalid_width;
}

// 写周期和占空比
void gpio_pulse_period_write(gpio_t *gpio, uint32_t period, float duty)
{
    ASSERT(gpio != NULL);

    if (gpio->flag.is_inited == false)
    {
        return;
    }

    if (period == 0 || duty == 0)
    {
        gpio->ops.write(0);
        gpio->pulse.valid_width = 0;
        gpio->pulse.invalid_width = 0;
    }
    gpio->pulse.start_point = TIMESTAMP_US_GET();
    gpio->pulse.valid_width = period * duty;
    gpio->pulse.invalid_width = period - gpio->pulse.valid_width;
}

// 写脉冲次数，不写就是无限次
void gpio_pulse_count_write(gpio_t *gpio, uint32_t count)
{
    ASSERT(gpio != NULL);

    if (gpio->flag.is_inited == false)
    {
        ERROR("[%s] gpio_pulse_count_write failed, gpio is not inited", gpio->cfg.name);
        return;
    }

    gpio->pulse.start_point = TIMESTAMP_US_GET();
    gpio->pulse.count = count;
    gpio->pulse.end_point = gpio->pulse.start_point + count * (gpio->pulse.valid_width + gpio->pulse.invalid_width);
}

// 写入有效值
void gpio_write(gpio_t *gpio, bool valid)
{
    ASSERT(gpio != NULL);

    if (gpio->flag.is_inited == false)
    {
        ERROR("[%s] gpio_write failed, gpio is not inited", gpio->cfg.name);
        return;
    }

    if (gpio_pulse_period_read(gpio) != 0)
    {
        gpio_pulse_period_write(gpio, gpio_pulse_period_read(gpio), valid ? 1.0f : 0.0f);
    }
    else
    {
        gpio_pulse_width_write(gpio, valid ? 0xFFFFFFFF : 0, valid ? 0 : 0xFFFFFFFF);
    }
    gpio->ops.write(valid ? gpio->cfg.value : 0);
}

void gpio_value_set(gpio_t *gpio, uint32_t value)
{
    gpio->cfg.value = value;
}

// 写入值
void gpio_value_write(gpio_t *gpio, uint32_t value)
{
    ASSERT(gpio != NULL);

    if (gpio->flag.is_inited == false)
    {
        ERROR("[%s] gpio_value_write failed, gpio is not inited", gpio->cfg.name);
        return;
    }

    gpio_value_set(gpio, value);
    gpio_value_update(gpio);
}

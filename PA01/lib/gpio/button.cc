#include "./gpio.h"

void button_poll(gpio_t *gpio)
{
    uint64_t timestamp_us = TIMESTAMP_US_GET();
    uint32_t value = gpio_read(gpio);

    if (gpio->button.last_value != value)
    {
        // 发布状态改变
        dds_publish(gpio, &(gpio->CHANGED), NULL);
        gpio->button.last_value = value;
        gpio->button.flag.keep = false; // 重置标志位
    }

    // 更新时间点
    if (value != 0)
    {
        if (gpio->button.start_point < gpio->button.end_point)
        {
            // 此刻按下: 之前是抬起状态
            dds_publish(gpio, &(gpio->REVERSED), NULL);
            dds_publish(gpio, &(gpio->TO_VALID), NULL);

            if (gpio->button.end_point - gpio->button.start_point > gpio->cfg.timegap_long_press)
            {
                // 长抬结束
                dds_publish(gpio, &(gpio->LONG_RELEASED), NULL);
            }
            else if (gpio->button.end_point - gpio->button.start_point > gpio->cfg.timegap_short_press)
            {
                // 短抬起过
                dds_publish(gpio, &(gpio->RELEASEED), NULL);
            }
        }
        gpio->button.start_point = timestamp_us;
    }
    else
    {
        if (gpio->button.start_point > gpio->button.end_point)
        {
            // 此刻抬起: 之前是按下状态
            dds_publish(gpio, &(gpio->REVERSED), NULL);
            dds_publish(gpio, &(gpio->TO_INVALID), NULL);

            if (gpio->button.start_point - gpio->button.end_point > gpio->cfg.timegap_long_press)
            {
                // 长按结束
                dds_publish(gpio, &(gpio->LONG_PRESSED), NULL);
            }
            else if (gpio->button.start_point - gpio->button.end_point > gpio->cfg.timegap_short_press)
            {
                // 短按下过
                dds_publish(gpio, &(gpio->PRESSED), NULL);
            }
        }
        gpio->button.end_point = timestamp_us;
    }

    // 判断状态
    if (gpio->button.start_point > gpio->button.end_point)
    {
        // 现在是按下状态
        if (gpio->button.start_point - gpio->button.end_point > gpio->cfg.timegap_long_press)
        {
            // 刚转变为长按状态
            if (gpio->button.flag.keep == false)
            {
                dds_publish(gpio, &(gpio->TO_KEEP_LONG_PRESS), NULL);
                gpio->button.flag.keep = true;
            }
            // 一直长按
            dds_publish(gpio, &(gpio->KEEP_LONG_PRESS), NULL);
        }
    }
    else if (gpio->button.start_point < gpio->button.end_point)
    {
        // 现在是抬起状态
        if (gpio->button.end_point - gpio->button.start_point > gpio->cfg.timegap_long_press)
        {
            // 刚转变为长释放状态
            if (gpio->button.flag.keep == false)
            {
                dds_publish(gpio, &(gpio->TO_KEEP_LONG_RELEASE), NULL);
                gpio->button.flag.keep = true;
            }
            // 一直抬起
            dds_publish(gpio, &(gpio->KEEP_LONG_RELEASE), NULL);
        }
    }
}

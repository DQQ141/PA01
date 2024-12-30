#include "./serial_led.h"

void serial_led_init(sled_t *led)
{
    ASSERT(led != NULL);
    ASSERT(led->cfg.name != NULL);
    ASSERT(led->cfg.led_num > 0);
    ASSERT(led->cfg.time_t0h > 0);
    ASSERT(led->cfg.time_t0l > 0);
    ASSERT(led->cfg.time_reset > 0);
    ASSERT(led->cfg.timer_period > 0);
    ASSERT(led->cfg.order < SERIAL_LED_ORDER_MAX);
    ASSERT(led->ops.init != NULL);
    ASSERT(led->ops.deinit != NULL);
    ASSERT(led->ops.start != NULL);

    // 计算数据
    led->pulse_reset_length = led->cfg.time_reset / (led->cfg.time_t0h + led->cfg.time_t0l) + 1;          // 计算重置延时所需的数据个数
    led->pulse_length = (led->cfg.led_num * 24 + led->pulse_reset_length);                                // 计算总共需要的数据个数
    led->pulse_t0h = led->cfg.timer_period * led->cfg.time_t0h / (led->cfg.time_t0h + led->cfg.time_t0l); // 计算pulse_t0h周期数
    led->pulse_t0l = led->cfg.timer_period - led->pulse_t0h;                                              // 计算pulse_t0l周期数

    // 分配内存
    if (led->pulse == NULL)
    {
        led->pulse = MALLOC(led->pulse_length * led->cfg.timer_bits / 8);
        ASSERT(led->pulse != NULL);
    }
    if (led->color == NULL)
    {
        led->color = MALLOC(sizeof(sled_color_t) * led->cfg.led_num);
        ASSERT(led->color != NULL);
    }

    // 初始化数据
    memset(led->pulse, 0, led->pulse_length * led->cfg.timer_bits / 8);
    memset(led->color, 0, sizeof(sled_color_t) * led->cfg.led_num);

    // 初始化
    led->ops.init();

    // 启动
    led->ops.start(led->pulse, led->pulse_length);

    led->flag.is_inited = true;

    // 初始化每个通道的颜色
    for (int i = 0; i < led->cfg.led_num; i++)
    {
        serial_led_color_set(led, i, led->color[i]);
    }

    // 初始化完成
    INFO("[%s] init success.", led->cfg.name);
}

// 释放资源
void serial_led_deinit(sled_t *led)
{
    ASSERT(led != NULL);
    ASSERT(led->ops.deinit != NULL);

    led->flag.is_inited = false;

    // 关闭设备
    led->ops.deinit();

    // 释放内存
    led->pulse = NULL;
    led->color = NULL;

    // 关闭设备完成
    INFO("[%s] deinit success.", led->cfg.name);
}

// 打印参数
void serial_led_args_print(sled_t *led)
{
    ASSERT(led != NULL);
    INFO("\r\n[%s]\r\n"
         "led_num = %lu\r\n"
         "timer_bits = %lu bit\r\n"
         "timer_period = %lu\r\n"
         "time_t0h = %lu ns\r\n"
         "time_t0l = %lu ns\r\n"
         "time_reset = %lu ns\r\n"
         "pulse_t0h = %lu\r\n"
         "pulse_t0l = %lu\r\n"
         "pulse_reset_length = %lu\r\n"
         "pulse_length = %lu",
         led->cfg.name,
         led->cfg.led_num,
         led->cfg.timer_bits,
         led->cfg.timer_period,
         led->cfg.time_t0h,
         led->cfg.time_t0l,
         led->cfg.time_reset,
         led->pulse_t0h,
         led->pulse_t0l,
         led->pulse_reset_length,
         led->pulse_length);
}

void serial_led_color_set(sled_t *led, uint32_t index, sled_color_t color)
{
    ASSERT(led != NULL);
    ASSERT(index < led->cfg.led_num);

    if (!led->flag.is_inited)
    {
        ERROR("[%s] color_set failed, serial led is not inited.", led->cfg.name);
        return;
    }

    uint8_t rgb[3];
    led->color[index] = color;
    switch (led->cfg.order)
    {
    case SERIAL_LED_ORDER_RGB:
        rgb[0] = color.rgb.r, rgb[1] = color.rgb.g, rgb[2] = color.rgb.b;
        break;
    case SERIAL_LED_ORDER_RBG:
        rgb[0] = color.rgb.r, rgb[1] = color.rgb.b, rgb[2] = color.rgb.g;
        break;
    case SERIAL_LED_ORDER_GRB:
        rgb[0] = color.rgb.g, rgb[1] = color.rgb.r, rgb[2] = color.rgb.b;
        break;
    case SERIAL_LED_ORDER_GBR:
        rgb[0] = color.rgb.g, rgb[1] = color.rgb.b, rgb[2] = color.rgb.r;
        break;
    case SERIAL_LED_ORDER_BRG:
        rgb[0] = color.rgb.b, rgb[1] = color.rgb.r, rgb[2] = color.rgb.g;
        break;
    case SERIAL_LED_ORDER_BGR:
        rgb[0] = color.rgb.b, rgb[1] = color.rgb.g, rgb[2] = color.rgb.r;
        break;
    default:
        ERROR("%s order error.", led->cfg.name);
        return;
        break;
    }

    // 填装数据
    if (led->cfg.timer_bits == 16)
    {
        uint16_t *pulse = (uint16_t *)(led->pulse);
        for (uint8_t i = 0; i < 8; i++)
        {
            pulse[led->pulse_reset_length + index * 24 + i] = (rgb[0] & (0x80 >> i)) ? led->pulse_t0l : led->pulse_t0h;
            pulse[led->pulse_reset_length + index * 24 + i + 8] = (rgb[1] & (0x80 >> i)) ? led->pulse_t0l : led->pulse_t0h;
            pulse[led->pulse_reset_length + index * 24 + i + 16] = (rgb[2] & (0x80 >> i)) ? led->pulse_t0l : led->pulse_t0h;
        }
    }
    else if (led->cfg.timer_bits == 32)
    {
        uint32_t *pulse = (uint32_t *)(led->pulse);
        for (uint8_t i = 0; i < 8; i++)
        {
            pulse[led->pulse_reset_length + index * 24 + i] = (rgb[0] & (0x80 >> i)) ? led->pulse_t0l : led->pulse_t0h;
            pulse[led->pulse_reset_length + index * 24 + i + 8] = (rgb[1] & (0x80 >> i)) ? led->pulse_t0l : led->pulse_t0h;
            pulse[led->pulse_reset_length + index * 24 + i + 16] = (rgb[2] & (0x80 >> i)) ? led->pulse_t0l : led->pulse_t0h;
        }
    }
    else
    {
        ERROR("%s timer_bits error.", led->cfg.name);
        return;
    }
}

// 设置HSV颜色
void serial_led_hsv_set(sled_t *led, uint32_t index, uint16_t hue, uint8_t saturation, uint8_t brightness)
{
    ASSERT(led != NULL);
    ASSERT(index < led->cfg.led_num);

    if (!led->flag.is_inited)
    {
        ERROR("[%s] hsv_set failed, serial led is not inited.", led->cfg.name);
        return;
    }
    // 计算颜色
    sled_color_t color = {0};
    uint8_t hi = (hue / 60) % 6;
    uint8_t f = (hue / 60.0 - hue / 60) * 255;
    uint8_t p = brightness * (255 - saturation) / 255;
    uint8_t q = brightness * (255 - f * saturation / 255) / 255;
    uint8_t t = brightness * (255 - (255 - f) * saturation / 255) / 255;

    switch (hi)
    {
    case 0:
        color.rgb.r = brightness;
        color.rgb.g = t;
        color.rgb.b = p;
        break;
    case 1:
        color.rgb.r = q;
        color.rgb.g = brightness;
        color.rgb.b = p;
        break;
    case 2:
        color.rgb.r = p;
        color.rgb.g = brightness;
        color.rgb.b = t;
        break;
    case 3:
        color.rgb.r = p;
        color.rgb.g = q;
        color.rgb.b = brightness;
        break;
    case 4:
        color.rgb.r = t;
        color.rgb.g = p;
        color.rgb.b = brightness;
        break;
    case 5:
        color.rgb.r = brightness;
        color.rgb.g = p;
        color.rgb.b = q;
        break;
    default:
        break;
    }
    serial_led_color_set(led, index, color);
}

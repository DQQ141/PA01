#include "./gpio.h"

#include "./button.cc" //按键模块
#include "./pulse.cc"  //脉冲模块

// 初始化
void gpio_init(gpio_t *gpio) // 长按时间间隔
{
    ASSERT(gpio != NULL);
    ASSERT(gpio->cfg.name != NULL);
    ASSERT(gpio->cfg.value != 0);
    ASSERT(gpio->ops.init != NULL);
    ASSERT(gpio->ops.read != NULL);
    ASSERT(gpio->ops.write != NULL);

    gpio->ops.init();
    gpio->ops.write(false);
    gpio->flag.is_inited = true;

    INFO("%s init succuss.", gpio->cfg.name);
}

// 轮询
void gpio_poll(gpio_t *gpio)
{
    if (!gpio->flag.is_inited)
    {
        return;
    }
    gpio->flag.is_running = true;

    pulse_poll(gpio);
    button_poll(gpio);
}

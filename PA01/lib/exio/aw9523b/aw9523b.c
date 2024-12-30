#include "./aw9523b.h"
#include <string.h>

// AW9523B寄存器定义
#define REG_GPIO_IN_P0 0x00
#define REG_GPIO_IN_P1 0x01
#define REG_GPIO_OUT_P0 0x02
#define REG_GPIO_OUT_P1 0x03
#define REG_GPIO_CFG_P0 0x04
#define REG_GPIO_CFG_P1 0x05
#define REG_INT_P0 0x06
#define REG_INT_P1 0x07
#define REG_ID 0x10
#define REG_CTL 0x11
#define REG_LED_MODE_P0 0x12
#define REG_LED_MODE_P1 0x13
#define REG_LED_DIM0_P1_0 0x20
#define REG_LED_DIM1_P1_1 0x21
#define REG_LED_DIM2_P1_2 0x22
#define REG_LED_DIM3_P1_3 0x23
#define REG_LED_DIM4_P0_0 0x24
#define REG_LED_DIM5_P0_1 0x25
#define REG_LED_DIM6_P0_2 0x26
#define REG_LED_DIM7_P0_3 0x27
#define REG_LED_DIM8_P0_4 0x28
#define REG_LED_DIM9_P0_5 0x29
#define REG_LED_DIM10_P0_6 0x2A
#define REG_LED_DIM11_P0_7 0x2B
#define REG_LED_DIM12_P1_4 0x2C
#define REG_LED_DIM13_P1_5 0x2D
#define REG_LED_DIM14_P1_6 0x2E
#define REG_LED_DIM15_P1_7 0x2F
#define REG_RESET 0x7F
#define AW9523B_CHIP_ID 0x23

// 初始化AW9523B
void aw9523b_init(aw9523b_t *aw9523b)
{
    ASSERT(aw9523b != NULL);
    ASSERT(aw9523b->cfg.name != NULL);
    ASSERT(aw9523b->cfg.try_cnt != 0);
    ASSERT(aw9523b->ops.init != NULL);
    ASSERT(aw9523b->ops.read != NULL);
    ASSERT(aw9523b->ops.write != NULL);
    ASSERT(aw9523b->ops.delay != NULL);

    // 初始化硬件接口
    aw9523b->ops.init();

    aw9523b->ops.write(REG_CTL, (uint8_t[]){0x01 << 4}, 1); // 设置所有IO为推挽模式

    aw9523b->init_try_cnt = 0;
    aw9523b->flag.value = 0;
}

void aw9523b_poll(aw9523b_t *aw9523b)
{
    // 初始化检查
    if (!aw9523b->flag.is_inited)
    {
        if (aw9523b->init_try_cnt < aw9523b->cfg.try_cnt)
        {
            // 复位芯片
            MUTEX_LOCK(&aw9523b->mutex);
            aw9523b->flag.is_inited = false;
            uint8_t reset_cmd = 0x00;
            aw9523b->ops.write(REG_RESET, &reset_cmd, 1);
            aw9523b->ops.delay();

            // 读取ID并打印
            uint8_t id = 0;
            aw9523b->ops.read(REG_ID, &id, 1);
            if (id == AW9523B_CHIP_ID)
            {
                // 读取输出寄存器
                aw9523b->ops.read(REG_GPIO_OUT_P1, &aw9523b->output.data[1], 1);
                aw9523b->ops.read(REG_GPIO_OUT_P0, &aw9523b->output.data[0], 1);

                // 读取配置寄存器
                aw9523b->ops.read(REG_GPIO_CFG_P1, &aw9523b->mask.data[1], 1);
                aw9523b->ops.read(REG_GPIO_CFG_P0, &aw9523b->mask.data[0], 1);

                // 初始化完成
                aw9523b->flag.is_inited = true;
                INFO("aw9523b chip ID: 0x%02X.", id);
                INFO("[%s] init success.", aw9523b->cfg.name);
            }
            MUTEX_UNLOCK(&aw9523b->mutex);
            aw9523b->init_try_cnt++;
        }
        else if (aw9523b->init_try_cnt == aw9523b->cfg.try_cnt)
        {
            ERROR("[%s] init failed.", aw9523b->cfg.name);
            aw9523b->init_try_cnt++;
        }
        return; // 如果未初始化完成，直接返回
    }
}

// 设置GPIO方向
void aw9523b_pin_config(aw9523b_t *aw9523b, uint8_t pin, bool is_output)
{
    ASSERT(aw9523b != NULL);
    ASSERT(pin < 16);

    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] pin_config failed, aw9523b not init.", aw9523b->cfg.name);
        return;
    }

    MUTEX_LOCK(&aw9523b->mutex);

    uint8_t reg = (pin < 8) ? REG_GPIO_CFG_P0 : REG_GPIO_CFG_P1;
    uint8_t *data = (pin < 8) ? &(aw9523b->mask.data[0]) : &(aw9523b->mask.data[1]);
    uint8_t bit = pin % 8;
    uint8_t mask = 1 << (bit);

    aw9523b->ops.read(reg, data, 1);
    if (is_output)
    {
        (*data) &= ~mask; // 设置为输出
    }
    else
    {
        (*data) |= mask; // 设置为输入
    }
    aw9523b->ops.write(reg, data, 1);

    MUTEX_UNLOCK(&aw9523b->mutex);
}

// 获取GPIO模式
bool aw9523b_pin_config_get(aw9523b_t *aw9523b, uint8_t pin)
{
    ASSERT(aw9523b != NULL);
    ASSERT(pin < 16);

    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] pin_config_get failed, aw9523b not init.", aw9523b->cfg.name);
        return false;
    }

    MUTEX_LOCK(&aw9523b->mutex);
    uint8_t reg = (pin < 8) ? REG_GPIO_CFG_P0 : REG_GPIO_CFG_P1;
    uint8_t *data = (pin < 8) ? &(aw9523b->mask.data[0]) : &(aw9523b->mask.data[1]);
    uint8_t bit = pin % 8;
    aw9523b->ops.read(reg, data, 1);
    MUTEX_UNLOCK(&aw9523b->mutex);
    return ((*data) & (1 << bit)) == 0;
}

// 获取GPIO掩码
uint16_t aw9523b_mask_read(aw9523b_t *aw9523b)
{
    ASSERT(aw9523b != NULL);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] mask_read failed, aw9523b not init.", aw9523b->cfg.name);
        return 0;
    }

    MUTEX_LOCK(&aw9523b->mutex);
    aw9523b->ops.read(REG_GPIO_CFG_P1, &(aw9523b->mask.data[1]), 1);
    aw9523b->ops.read(REG_GPIO_CFG_P0, &(aw9523b->mask.data[0]), 1);
    MUTEX_UNLOCK(&aw9523b->mutex);
    return aw9523b->mask.value;
}

// 设置GPIO掩码
void aw9523b_mask_write(aw9523b_t *aw9523b, uint16_t gpio_mask)
{
    ASSERT(aw9523b != NULL);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] mask_write failed, aw9523b not init.", aw9523b->cfg.name);
        return;
    }

    MUTEX_LOCK(&aw9523b->mutex);
    aw9523b->mask.value = gpio_mask;
    aw9523b->ops.write(REG_GPIO_CFG_P1, &(aw9523b->mask.data[1]), 1);
    aw9523b->ops.write(REG_GPIO_CFG_P0, &(aw9523b->mask.data[0]), 1);
    MUTEX_UNLOCK(&aw9523b->mutex);
}

// 读取GPIO值
uint16_t aw9523b_input_read(aw9523b_t *aw9523b)
{
    ASSERT(aw9523b != NULL);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] input_read failed, aw9523b not init.", aw9523b->cfg.name);
    }

    MUTEX_LOCK(&aw9523b->mutex);
    uint8_t data[2] = {0};
    aw9523b->ops.read(REG_GPIO_IN_P1, &(data[1]), 1);
    aw9523b->ops.read(REG_GPIO_IN_P0, &(data[0]), 1);
    MUTEX_UNLOCK(&aw9523b->mutex);
    return (data[1] << 8) | data[0];
}

// 读取GPIO值
uint16_t aw9523b_output_read(aw9523b_t *aw9523b)
{
    ASSERT(aw9523b != NULL);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] output_read failed, aw9523b not init.", aw9523b->cfg.name);
    }

    MUTEX_LOCK(&aw9523b->mutex);
    aw9523b->ops.read(REG_GPIO_OUT_P1, &(aw9523b->output.data[1]), 1);
    aw9523b->ops.read(REG_GPIO_OUT_P0, &(aw9523b->output.data[0]), 1);
    MUTEX_UNLOCK(&aw9523b->mutex);
    return aw9523b->output.value;
}

// 设置GPIO值
void aw9523b_write(aw9523b_t *aw9523b, uint16_t value)
{
    ASSERT(aw9523b != NULL);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] write failed, aw9523b not init.", aw9523b->cfg.name);
    }

    MUTEX_LOCK(&aw9523b->mutex);
    aw9523b->output.value = value;
    aw9523b->ops.write(REG_GPIO_OUT_P1, &(aw9523b->output.data[1]), 1);
    aw9523b->ops.write(REG_GPIO_OUT_P0, &(aw9523b->output.data[0]), 1);
    MUTEX_UNLOCK(&aw9523b->mutex);
}

// 设置GPIO PIN值
void aw9523b_pin_write(aw9523b_t *aw9523b, uint8_t pin, bool is_high)
{
    ASSERT(aw9523b != NULL);
    ASSERT(pin < 16);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] pin_write failed, aw9523b not init.", aw9523b->cfg.name);
    }

    MUTEX_LOCK(&aw9523b->mutex);
    uint8_t reg = (pin < 8) ? REG_GPIO_OUT_P0 : REG_GPIO_OUT_P1;
    uint8_t *data = (pin < 8) ? &(aw9523b->output.data[0]) : &(aw9523b->output.data[1]);
    uint8_t bit = pin % 8;
    uint8_t mask = 1 << bit;

    aw9523b->ops.read(reg, data, 1);
    if (is_high)
    {
        (*data) |= mask; // 设置为高
    }
    else
    {
        (*data) &= ~mask; // 设置为低
    }
    aw9523b->ops.write(reg, data, 1);
    MUTEX_UNLOCK(&aw9523b->mutex);
}

// 获取GPIO值
bool aw9523b_input_pin_read(aw9523b_t *aw9523b, uint8_t pin)
{
    ASSERT(aw9523b != NULL);
    ASSERT(pin < 16);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] input_pin_read failed, aw9523b not init.", aw9523b->cfg.name);
    }

    MUTEX_LOCK(&aw9523b->mutex);
    uint8_t reg = (pin < 8) ? REG_GPIO_IN_P0 : REG_GPIO_IN_P1;
    uint8_t *data = (pin < 8) ? &(aw9523b->output.data[0]) : &(aw9523b->output.data[1]);
    uint8_t bit = pin % 8;
    aw9523b->ops.read(reg, data, 1);
    MUTEX_UNLOCK(&aw9523b->mutex);
    return ((*data) & (1 << bit)) != 0;
}

// 获取GPIO值
bool aw9523b_output_pin_read(aw9523b_t *aw9523b, uint8_t pin)
{
    ASSERT(aw9523b != NULL);
    ASSERT(pin < 16);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] output_pin_read failed, aw9523b not init.", aw9523b->cfg.name);
    }

    MUTEX_LOCK(&aw9523b->mutex);
    uint8_t reg = (pin < 8) ? REG_GPIO_OUT_P0 : REG_GPIO_OUT_P1;
    uint8_t *data = (pin < 8) ? &(aw9523b->output.data[0]) : &(aw9523b->output.data[1]);
    uint8_t bit = pin % 8;
    aw9523b->ops.read(reg, data, 1);
    MUTEX_UNLOCK(&aw9523b->mutex);
    return ((*data) & (1 << bit)) != 0;
}

// 设置LED亮度
void aw9523b_brightness_write(aw9523b_t *aw9523b, uint8_t led, uint8_t brightness)
{
    ASSERT(aw9523b != NULL);
    ASSERT(led < 16);
    ASSERT(brightness <= 255);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] brightness_write failed, aw9523b not init.", aw9523b->cfg.name);
    }

    MUTEX_LOCK(&aw9523b->mutex);
    uint8_t reg = REG_LED_DIM0_P1_0 + led;
    aw9523b->ops.write(reg, &brightness, 1);
    aw9523b->brightness[led] = brightness;
    MUTEX_UNLOCK(&aw9523b->mutex);
}

// 读取LED亮度
uint8_t aw9523b_brightness_read(aw9523b_t *aw9523b, uint8_t led)
{
    ASSERT(aw9523b != NULL);
    ASSERT(led < 16);

    return aw9523b->brightness[led];
}

// 设置LED电流限制
void aw9523b_imax_write(aw9523b_t *aw9523b, uint8_t imax)
{
    ASSERT(aw9523b != NULL);
    ASSERT(imax <= 3);
    if (aw9523b->flag.is_inited == false)
    {
        ERROR("[%s] imax_write failed, aw9523b not init.", aw9523b->cfg.name);
    }

    MUTEX_LOCK(&aw9523b->mutex);
    uint8_t reg_value;
    aw9523b->ops.read(REG_CTL, &reg_value, 1);
    reg_value &= ~0x03; // 清除当前设置
    reg_value |= imax;  // 设置新的电流限制
    aw9523b->ops.write(REG_CTL, &reg_value, 1);
    aw9523b->imax = imax;
    MUTEX_UNLOCK(&aw9523b->mutex);
}

// 读取LED电流限制
uint8_t aw9523b_imax_read(aw9523b_t *aw9523b)
{
    ASSERT(aw9523b != NULL);
    return aw9523b->imax;
}

/**
 * @file aw9523b.cc
 * @author WittXie
 * @brief aw9523b IO拓展芯片端口
 * @version 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../aw9523b/aw9523b_bsp.h"
#include "./../gpio_bsp.h"
static uint16_t s_aw9523b_buttonboard = 0;               // 存储扫描到的按键结果
static const uint8_t s_input_pins[] = {0, 1, 2, 3};      // 按键输入引脚 P0-P3
static const uint8_t s_output_pins[] = {10, 11, 12, 13}; // 按键输出引脚 P10-P13
static void button_scan_entry(void *args)
{
    uint16_t BUTTON_INPUT_MASK = 0;
    uint16_t BUTTON_OUTPUT_MASK = 0;

    // 设置引脚MASK
    for (uint8_t i = 0; i < countof(s_input_pins); i++)
    {
        BUTTON_INPUT_MASK = bit_set(BUTTON_INPUT_MASK, s_input_pins[i]);
        BUTTON_OUTPUT_MASK = bit_set(BUTTON_OUTPUT_MASK, s_output_pins[i]);
    }

    // 初始化：设置行引脚为输入，列引脚为输出
    uint16_t mask = g_aw9523b_exio.mask.value;
    mask |= BUTTON_INPUT_MASK;   // 设置BUTTONINPUT为输入
    mask &= ~BUTTON_OUTPUT_MASK; // 设置BUTTONOUTPUT为输出
    aw9523b_mask_write(&g_aw9523b_exio, mask);
    aw9523b_write(&g_aw9523b_exio, g_aw9523b_exio.output.value & ~BUTTON_OUTPUT_MASK); // 设置BUTTONOUTPUT为输出高电平

    uint16_t output_value = g_aw9523b_exio.output.value;
    uint16_t input_value = 0;
    for (;;)
    {
        if (aw9523b_is_inited(&g_aw9523b_exio) == false)
        {
            os_sleep(500);
            continue;
        }

        for (uint8_t row = 0; row < countof(s_input_pins); row++) // 扫描行
        {
            output_value = g_aw9523b_exio.output.value | BUTTON_OUTPUT_MASK; // 重置IO为高电平
            output_value = bit_reset(output_value, s_output_pins[row]);      // 清除当前列为低电平
            aw9523b_write(&g_aw9523b_exio, output_value);                    // 设置当前列为低电平
            os_sleep(10);

            // 扫描列
            input_value = aw9523b_input_read(&g_aw9523b_exio);
            for (uint8_t col = 0; col < countof(s_input_pins); col++)
            {
                if (bit_read(input_value, s_input_pins[col]) == 0)
                {
                    s_aw9523b_buttonboard = bit_set(s_aw9523b_buttonboard, col * countof(s_input_pins) + row);
                }
                else
                {
                    s_aw9523b_buttonboard = bit_reset(s_aw9523b_buttonboard, col * countof(s_input_pins) + row);
                }
            }
        }
    }
}

static void aw9523b_button_scan_init(void)
{
    os_task_create(button_scan_entry, "button_scan", NULL, OS_PRIORITY_HIGHEST, OS_TASK_STACK_MIN + 512);
}
static void aw9523b_null_init(void)
{
}
static uint32_t aw9523b_null_read(void)
{
    return 0;
}
static void aw9523b_null_write(uint32_t value)
{
}
#define MAKE_AW9523B_BUTTON(_NAME, _ROW, _COL)                                           \
    static uint32_t __##_NAME##_read(void)                                               \
    {                                                                                    \
        return bit_read(s_aw9523b_buttonboard, (_COL) * countof(s_input_pins) + (_ROW)); \
    }                                                                                    \
    gpio_t _NAME = {                                                                     \
        .cfg = {                                                                         \
            .name = DEFINE_TO_STR(_NAME),                                                \
            .timegap_short_press = 30000,                                                \
            .timegap_long_press = 300000,                                                \
            .value = 1,                                                                  \
        },                                                                               \
        .ops = {                                                                         \
            .init = &aw9523b_null_init,                                                  \
            .read = &__##_NAME##_read,                                                   \
            .write = &aw9523b_null_write,                                                \
        },                                                                               \
    }

#define MAKE_AW9523B_OUT(_NAME, _PIN, _REVERSE)                                  \
    static void __##_NAME##_init(void)                                           \
    {                                                                            \
        aw9523b_pin_config(&g_aw9523b_exio, (_PIN), true);                       \
    }                                                                            \
    static uint32_t __##_NAME##_read(void)                                       \
    {                                                                            \
        return bit_read(g_aw9523b_exio.output.value, (_PIN));                    \
    }                                                                            \
    static void __##_NAME##_write(uint32_t value)                                \
    {                                                                            \
        aw9523b_pin_write(&g_aw9523b_exio, (_PIN), (_REVERSE) ? !value : value); \
    }                                                                            \
    gpio_t _NAME = {                                                             \
        .cfg = {                                                                 \
            .name = DEFINE_TO_STR(_NAME),                                        \
            .timegap_short_press = 30000,                                        \
            .timegap_long_press = 300000,                                        \
            .value = 1,                                                          \
        },                                                                       \
        .ops = {                                                                 \
            .init = &__##_NAME##_init,                                           \
            .read = &__##_NAME##_read,                                           \
            .write = &__##_NAME##_write,                                         \
        },                                                                       \
    }

// BUTTON
// TR1- TR1+ TR2- TR2+
// TR3- TR3+ TR4- TR4+
// P+   P-   RTN  MENU
// BUTTON1 BUTTON2 BUTTON3 BUTTON4
MAKE_AW9523B_BUTTON(g_exbtn_tr1n, 0, 0);
MAKE_AW9523B_BUTTON(g_exbtn_tr1p, 0, 1);
MAKE_AW9523B_BUTTON(g_exbtn_tr2n, 0, 2);
MAKE_AW9523B_BUTTON(g_exbtn_tr2p, 0, 3);
MAKE_AW9523B_BUTTON(g_exbtn_tr3n, 1, 0);
MAKE_AW9523B_BUTTON(g_exbtn_tr3p, 1, 1);
MAKE_AW9523B_BUTTON(g_exbtn_tr4n, 1, 2);
MAKE_AW9523B_BUTTON(g_exbtn_tr4p, 1, 3);
MAKE_AW9523B_BUTTON(g_exbtn_pp, 2, 0);
MAKE_AW9523B_BUTTON(g_exbtn_pn, 2, 1);
MAKE_AW9523B_BUTTON(g_exbtn_rtn, 2, 2);
MAKE_AW9523B_BUTTON(g_exbtn_menu, 2, 3);
MAKE_AW9523B_BUTTON(g_exbtn_button1, 3, 0);
MAKE_AW9523B_BUTTON(g_exbtn_button2, 3, 1);
MAKE_AW9523B_BUTTON(g_exbtn_button3, 3, 2);
MAKE_AW9523B_BUTTON(g_exbtn_button4, 3, 3);

// EXOUT
MAKE_AW9523B_OUT(g_exout_charge_enable, 8, false);     // 充电使能, P1-0 = P8, 高电平有效
MAKE_AW9523B_OUT(g_exout_gpio0, 9, false);             // GPIO0, P1-1 = P9, 高电平有效
MAKE_AW9523B_OUT(g_exout_u6_select_exrf, 14, true);    // U6选择, P1-6 = P14, 低电平选择外置高频头, 高电平选择IBUS2
MAKE_AW9523B_OUT(g_exout_rfpower_enable, 7, false);    // 内部RF电源使能, P0-7 = P07, 高电平有效
MAKE_AW9523B_OUT(g_exout_exrfpower_enable, 15, false); // 外部RF电源使能, P1-7 = P15, 高电平有效

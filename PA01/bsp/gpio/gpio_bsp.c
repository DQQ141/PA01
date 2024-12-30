#include "./gpio_bsp.h"

// 驱动加载
#include "./../../lib/gpio/gpio.c"

// 端口加载
#include "./port/aw9523b.cc" // AW9523B IO拓展芯片
#include "./port/gpio.cc"    // 普通GPIO
#include "./port/pwm.cc"     // PWM
#include "./port/rgb.cc"     // RGB灯

// 设备组
gpio_t *const g_gpio_group[] = {
    &g_rgb_left1,
    &g_rgb_left2,
    &g_rgb_power1,
    &g_rgb_power2,
    &g_rgb_11,
    &g_rgb_12,
    &g_rgb_21,
    &g_rgb_22,
    &g_rgb_31,
    &g_rgb_32,
    &g_rgb_41,
    &g_rgb_42,
    &g_rgb_right2,
    &g_rgb_right1,
    &g_rgb_power,
    &g_rgb_power,

    &g_btn_slw_left,
    &g_btn_roller,
    &g_btn_right,
    &g_btn_power,
    &g_btn_dfu,
    &g_btn_back_left,
    &g_btn_back_right,
    &g_exbtn_tr1n,
    &g_exbtn_tr1p,
    &g_exbtn_tr2n,
    &g_exbtn_tr2p,
    &g_exbtn_tr3n,
    &g_exbtn_tr3p,
    &g_exbtn_tr4p,
    &g_exbtn_tr4n,
    &g_exbtn_pp,
    &g_exbtn_pn,
    &g_exbtn_rtn,
    &g_exbtn_menu,
    &g_exbtn_button1,
    &g_exbtn_button2,
    &g_exbtn_button3,
    &g_exbtn_button4,

    &g_out_power,
    &g_exout_charge_enable,
    &g_exout_gpio0,
    &g_exout_u6_select_exrf,
    &g_exout_rfpower_enable,
    &g_exout_exrfpower_enable,
    &g_pwm_vibration,
    &g_pwm_lcd_light,
};
const uint32_t GPIO_GROUP_SIZE = countof(g_gpio_group);

static void gpio_entry(void *args)
{
    os_rely_wait(aw9523b_is_inited(&g_aw9523b_exio), 6000);  // 依赖 aw9523b
    os_rely_wait(serial_led_is_inited(&g_serial_led), 6000); // 依赖 serial_led

    if (aw9523b_is_inited(&g_aw9523b_exio) == false || serial_led_is_inited(&g_serial_led) == false)
    {
        os_return;
    }

    // 初始化
    for (int i = 0; i < GPIO_GROUP_SIZE; i++)
    {
        gpio_init(g_gpio_group[i]);
        os_sleep(1);
    }

    // 启动按键扫描
    aw9523b_button_scan_init();
    log_info("gpio init done.");

    // 监听
    for (;;)
    {
        os_sleep(10);
        for (int i = 0; i < GPIO_GROUP_SIZE; i++)
        {
            gpio_poll(g_gpio_group[i]);
        }
    }
}
void gpio_bsp_init(void)
{
    os_task_create(gpio_entry, "gpio", NULL, (OS_PRIORITY_BSP + OS_PRIORITY_REALTIME) / 2, OS_TASK_STACK_MIN + 1024 * 6);
}

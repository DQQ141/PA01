#include "./test_app.h"

// 加载测试
#include "./adc/adc_test.cc"
#include "./aw9523b/aw9523b_test.cc"
#include "./button/button_test.cc"
#include "./lcd/lcd_test.cc"
#include "./led/led_test.cc"
#include "./list/list_test.cc"
#include "./lsm6dsdtr/lsm6dsdtr_test.cc"
#include "./lvgl/lvgl_test.cc"
#include "./nop/nop_test.cc"
#include "./ppm/ppm_test.cc"
#include "./protocol/protocol_test.cc"
#include "./qflash/qflash_test.cc"
#include "./ram/ram_test.cc"
#include "./rf_power/rf_power_test.cc"
#include "./roller/roller_test.cc"
#include "./sdcard/sdcard_test.cc"
#include "./sdram/sdram_test.cc"
#include "./stick/stick_test.cc"

// 测试任务
static void test_entry(void *args)
{
    // 依赖 gpio
    for (int i = 0; i < GPIO_GROUP_SIZE; i++)
    {
        os_rely_wait(gpio_is_running(g_gpio_group[i]), 6000);
    }
    for (int i = 0; i < PROTOCOL_GROUP_SIZE; i++)
    {
        os_rely_wait(protocol_is_running(g_protocol_group[i]), 6000);
    }
    os_rely_wait(roller_is_running(&g_roller), 6000);           // 依赖旋钮
    os_rely_wait(lvgl_is_running(), 6000);                      // 依赖 lvgl
    os_rely_wait(adc_is_running(), 6000);                       // 依赖ADC
    os_rely_wait(fs_stick_is_running(&g_fs_stick), 6000);       // 依赖摇杆
    os_rely_wait(lsm6dsdtr_is_running(&g_lsm6dsdtr_imu), 6000); // 依赖陀螺仪

    // 显示系统状态
    // os_monitor_pagesize_set(20);
    // os_monitor_current_page_set(0);
    // os_monitor_display(true);
    bsp_print_debug_mode(); // 打印调试模式

    // 测试模块
    // aw9523b_test(); // aw9523b IO拓展模块
    button_test();
    // lcd_test();
    // lvgl_test();
    // led_test();
    // lsm6dsdtr_test(); // 陀螺仪
    // nop_test();
    // ppm_test();
    qflash_test();
    // ram_test();
    // sdcard_test();
    // sdram_test();
    // roller_test();
    // stick_test();
    // adc_test();
    // protocol_test();
    // list_test();
    // rf_power_test();

    // 循环
    for (;;)
    {
        os_sleep(1000);
    }
}

// 初始化测试
void test_app_init(void)
{
    os_task_create(test_entry, "test", NULL, OS_PRIORITY_APP, OS_TASK_STACK_MIN + 2048);
}

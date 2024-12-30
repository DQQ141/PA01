#include "./lvgl_bsp.h"

static bool lvgl_running_flag = false;
void *lvgl_mutex = NULL;

// 屏幕刷新
void lvgl_bsp_flush(void)
{
    gc9307c_frame_count_hook(&g_gc9307c_lcd, TIMESTAMP_US_GET());
    lv_task_handler(); // lvgl
}

bool lvgl_is_running(void)
{
    return lvgl_running_flag;
}

// 打印输出hook
void lvgl_print(const char *format)
{

    if (strstr(format, "[Error]\r\n"))
    {
        print(COLOR_H_RED "lvgl: %s", format);
    }
    else if (strstr(format, "[Warn]"))
    {
        print(COLOR_H_YELLOW "lvgl: %s\r\n", format);
    }
    else if (strstr(format, "[Info]\r\n"))
    {
        print(COLOR_H_GREEN "lvgl: %s\r\n", format);
    }
    else if (strstr(format, "[Trace]"))
    {
        print(COLOR_H_WHITE "lvgl: %s", format);
    }
    else if (strstr(format, "[User]"))
    {
        print(COLOR_H_BLUE "lvgl: %s\r\n", format);
    }
    else
    {
        print(COLOR_H_WHITE "lvgl: %s\r\n", format);
    }
}

static void lvgl_entry(void *args)
{
    // 依赖 g_gc9307c
    os_rely_wait(gc9307c_is_inited(&g_gc9307c_lcd), 6000);
    if (gc9307c_is_inited(&g_gc9307c_lcd) == false)
    {
        os_return;
    }

    // 初始化LVGL
    lv_init();
    lv_log_register_print_cb(lvgl_print);
    // 初始化LVGL显示端口
    void lv_port_disp_init(void);
    lv_port_disp_init();
    lvgl_running_flag = true;

    os_sleep(200);

    for (;;)
    {
        // 等待FMARK同步信号
        os_sleep_until(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET, 1000);

        // 刷新屏幕
        lvgl_lock();
        time_invoke(2000000, { // 每2秒给lcd重新刷一次寄存器，防止静电导致的屏幕异常
            // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET); // 拉低复位引脚
            // os_sleep(5);
            // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET); // 拉高复位引脚
            // os_sleep(5);
            gc9307c_reg_init(&g_gc9307c_lcd);
        });
        lvgl_bsp_flush();
        lvgl_unlock();
    }
}

// BSP初始化
void lvgl_bsp_init(void)
{
    os_task_create(lvgl_entry, "lvgl", NULL, OS_PRIORITY_LOWEST, OS_TASK_STACK_MIN + 4096);
}

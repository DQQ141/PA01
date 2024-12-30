/**
 * @file lcd_test.cc
 * @author WittXie
 * @brief 屏幕测试
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void lcd_entry(void *args)
{
    uint16_t color[] = {
        GC9307C_COLOR_BLACK,
        GC9307C_COLOR_WHITE,
        GC9307C_COLOR_RED,
        GC9307C_COLOR_GREEN,
        GC9307C_COLOR_BLUE,
        GC9307C_COLOR_YELLOW,
        GC9307C_COLOR_CYAN,
        GC9307C_COLOR_MAGENTA,
        GC9307C_COLOR_ORANGE,
        GC9307C_COLOR_PURPLE,
    };
    uint8_t cnt = 0;
    for (;;)
    {
        os_sleep(1000);
        gc9307c_clear(g_gc9307c_group[0], color[cnt]);
        cnt++;
        cnt %= countof(color);
    }
}
static void lcd_test(void)
{
    os_task_create(lcd_entry, "lcd_test", NULL, OS_PRIORITY_APP, OS_TASK_STACK_MIN + 2048);
}

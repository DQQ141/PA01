/**
 * @file lcd_aging_test_script.cc
 * @author WittXie
 * @brief 屏幕老化脚本
 * @version 0.1
 * @date 2024-11-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../script_app.h"
struct __page_lcd_screen
{
    uint8_t current; // 当前显示的屏幕
    uint8_t page_size;
};

static void lcd_aging_slw_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    factory_page_t *current_page;
    struct __page_lcd_screen *page_lcd_screen = (struct __page_lcd_screen *)g_page_lcd_screen.userdata;

    time_invoke(100000, { // 100ms执行一次
        current_page = g_factory.lvgl.page_path[g_factory.lvgl.page_index];
        if (current_page != NULL)
        {
            if (current_page == &g_page_home)
            {
                ui_hal_page_entry(&g_page_lcd_screen); // 进入屏幕测试
            }
            else if (current_page == &g_page_testbox)
            {
                ui_hal_page_goback(FACTORY_PAGE_HOME); // 返回首页
            }
            else if (current_page == &g_page_msgbox)
            {
                ui_hal_page_goback(FACTORY_PAGE_HOME); // 返回首页
            }
        }

        time_invoke(3000000, { // 3s执行一次
            // 测试中
            if (current_page == &g_page_lcd_screen)
            {
                if (page_lcd_screen->current == 4)
                {
                    int32_t value = 1;
                    dds_publish(&g_roller, &g_roller.CHANGED, &value); // 模拟滚轮下滑
                }
                else if (page_lcd_screen->current == 5)
                {
                    int32_t value = -1;
                    dds_publish(&g_roller, &g_roller.CHANGED, &value); // 模拟滚轮上滑
                }
                else
                {
                    page_lcd_screen->current = 4;
                    int32_t value = 1;
                    dds_publish(&g_roller, &g_roller.CHANGED, &value); // 模拟滚轮下滑
                }
            }
        });

    });
}

static void lcd_aging_test_script_inti(void)
{
    dds_subcribe(&g_btn_slw_left.KEEP_LONG_PRESS, DDS_PRIORITY_NORMAL, lcd_aging_slw_callback, NULL); // 绑定左自锁按钮的长按事件
}

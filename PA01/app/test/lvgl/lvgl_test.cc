/**
 * @file lvgl_test.cc
 * @author WittXie
 * @brief LVGL测试
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void lvgl_entry(void *args)
{
    os_sleep(1000);
    lv_obj_t *scr = lv_scr_act();                                   // 获取当前活动屏幕
    lv_obj_set_style_bg_color(scr, lv_color_black(), LV_PART_MAIN); // 设置背景颜色为黑色
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);       // 设置不透明度为完全不透明

    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_t *label = lv_label_create(btn);
    lv_obj_set_size(btn, 120, 50);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    int8_t cnt = 0;
    int8_t dir = 1;
    for (;;)
    {
        os_sleep(20);
        lv_obj_set_pos(btn, 50 + cnt, 50 + cnt);

        cnt += dir;
        if (cnt > 100 || cnt < 0)
        {
            dir = -dir;
            cnt += dir;
        }
    }
}

static void lvgl_test(void)
{
    os_task_create(lvgl_entry, "lvgl_test", NULL, OS_PRIORITY_LOWEST, OS_TASK_STACK_MIN + 2048);
}

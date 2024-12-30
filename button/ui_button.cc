/**
 * @file ui_button.cc
 * @author WittXie
 * @brief UI响应按键
 * @version 0.1
 * @date 2024-11-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../../factory_app.h"

static void ui_roller_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    int32_t step = *(int32_t *)arg;
    roller_t *roller = (roller_t *)device;

    lvgl_lock();
    lv_obj_t *obj_focused = lv_group_get_focused(g_factory.lvgl.group);
    lvgl_unlock();

    if (obj_focused == NULL)
    {
        return;
    }

    if (step > 0)
    {
        for (int i = 0; i < step; i++)
        {
            if (obj_focused != NULL)
            {
                lvgl_lock();
                lv_group_focus_next(g_factory.lvgl.group); // 切换到下一个按钮
                lvgl_unlock();
                os_sleep(20);
            }
        }
    }
    else
    {
        for (int i = 0; i < -step; i++)
        {
            if (obj_focused != NULL)
            {
                lvgl_lock();
                lv_group_focus_prev(g_factory.lvgl.group); // 切换到上一个按钮
                lvgl_unlock();
                os_sleep(20);
            }
        }
    }
}

static void ui_roller_btn_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    lvgl_lock();
    gpio_t *btn = (gpio_t *)device;
    lv_obj_t *obj_focused = lv_group_get_focused(g_factory.lvgl.group);
    if (obj_focused != NULL)
    {
        lv_event_send(obj_focused, LV_EVENT_PRESSED, NULL);
        lv_event_send(obj_focused, LV_EVENT_RELEASED, NULL);
        lv_obj_add_state(obj_focused, LV_STATE_CHECKED);
    }
    lvgl_unlock();
}
static void ui_home_btn_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    lvgl_lock();
    ui_hal_page_goback(FACTORY_PAGE_HOME); // 回到主页
    lvgl_unlock();
}

static void ui_button_init(void)
{
    // 订阅
    dds_subcribe(&g_roller.CHANGED, DDS_PRIORITY_NORMAL, &ui_roller_callback, NULL);
    dds_subcribe(&g_btn_roller.PRESSED, DDS_PRIORITY_NORMAL, &ui_roller_btn_callback, NULL); // 回车
    dds_subcribe(&g_exbtn_rtn.PRESSED, DDS_PRIORITY_NORMAL, &ui_home_btn_callback, NULL);    // 返回主页
}

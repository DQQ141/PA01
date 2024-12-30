/**
 * @file factory_app.h
 * @author WittXie
 * @brief 工厂模式应用
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

// 依赖
#include "../app_env.h"
#include "./ui/lvgl/generated/events_init.h"
#include "./ui/lvgl/generated/gui_guider.h"

// 模块
#include "./rf/factory_rf.h"
#include "./ui/factory_ui.h"

#define FACTORY_PAGE_MAX 64 // 页面最大数量

#pragma pack(4)
typedef struct __factory_data
{
    union
    {
        struct
        {
            bool is_finished : 1;
            bool is_pass : 1;
        } result[FACTORY_PAGE_MAX];
    }; // 测试结果

    int8_t battery_shift;
    bool usb_is_pass;
    bool ppm_is_pass;
    bool head_is_pass;
    bool sport_is_pass;
    int16_t stick_shift[4]; // 单位us
} factory_data_t;           // 数据掉电保存
#pragma pack()

typedef struct __factory_app
{
    struct
    {
        bool is_running;  // 是否运行
        bool is_charging; // 是否充电
    } status;

    struct
    {
        float battery;   // 电量(断开电源后测得)0~1
        float volume;    // 音量: 0 ~ 1
        float vibration; // 震动：0 ~ 1
        float lcd_light; // 背光：0 ~ 1
    } value;

    factory_data_t data;    // 需要存储的数据
    const char *hw_version; // 硬件版本
    const char *sw_version; // 软件版本

    struct
    {
        lv_ui *ui;             // GUIDER生成的所有成员
        lv_group_t *group;     // 默认组
        lv_obj_t *obj_focused; // 当前焦点

        factory_page_t *page_path[FACTORY_PAGE_DEEP]; // 页面路径表
        uint8_t page_index;                           // 页面当前深度
        list_t page_list;                             // 页面链表
        uint16_t page_num;                            // 页面数量
    } lvgl;

} factory_app_t;

extern factory_app_t g_factory;

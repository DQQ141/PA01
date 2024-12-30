/**
 * @file timer.cc
 * @author WittXie
 * @brief 定时器
 * @version 0.1
 * @rtc_data 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../time_bsp.h"

static void rtc_time_set(void)
{
    RTC_DateTypeDef hal_rtc_date;
    RTC_TimeTypeDef hal_rtc_time;

    // 获取RTC时间
    HAL_RTC_GetTime(&hrtc, &hal_rtc_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &hal_rtc_date, RTC_FORMAT_BIN);

    // 转换RTC到时间戳
    vdate_t rtc_data = {
        .year = hal_rtc_date.Year + 2000,
        .month = hal_rtc_date.Month,
        .day = hal_rtc_date.Date,
        .hour = hal_rtc_time.Hours,
        .min = hal_rtc_time.Minutes,
        .sec = hal_rtc_time.Seconds,
        .ms = 0,
        .us = 0,
    };

    current_date_set(&g_time_timer5, &rtc_data);
}
static void build_time_set(void)
{
    // 获取编译时间
    vdate_t build_date = build_date_get();
    current_date_set(&g_time_timer5, &build_date);

    RTC_TimeTypeDef hal_rtc_time = {
        .Hours = build_date.hour,
        .Minutes = build_date.min,
        .Seconds = build_date.sec,
        .TimeFormat = RTC_HOURFORMAT12_AM,
        .DayLightSaving = RTC_DAYLIGHTSAVING_NONE,
        .StoreOperation = RTC_STOREOPERATION_RESET,
    };
    HAL_RTC_SetTime(&hrtc, &hal_rtc_time, RTC_FORMAT_BIN);

    RTC_DateTypeDef hal_rtc_date = {
        .Year = build_date.year - 2000,
        .Month = build_date.month,
        .Date = build_date.day,
        .WeekDay = 0,
    };
    HAL_RTC_SetDate(&hrtc, &hal_rtc_date, RTC_FORMAT_BIN);
}

static void time_auto_set(void)
{
    RTC_DateTypeDef hal_rtc_date;
    RTC_TimeTypeDef hal_rtc_time;

    // 获取RTC时间
    HAL_RTC_GetTime(&hrtc, &hal_rtc_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &hal_rtc_date, RTC_FORMAT_BIN);

    // 转换RTC到时间戳
    vdate_t rtc_data = {
        .year = hal_rtc_date.Year + 2000,
        .month = hal_rtc_date.Month,
        .day = hal_rtc_date.Date,
        .hour = hal_rtc_time.Hours,
        .min = hal_rtc_time.Minutes,
        .sec = hal_rtc_time.Seconds,
        .ms = 0,
        .us = 0,
    };

    // 获取编译时间
    vdate_t build_date = build_date_get();

    if (date_to_timestamp(&build_date) > date_to_timestamp(&rtc_data))
    {
        build_time_set();
    }
    else
    {
        rtc_time_set();
    }
}

static void timer5_init(void)
{
    time_auto_set();

    // 启用定时器
    __HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_UPDATE);
    HAL_TIM_Base_Start_IT(&htim5);
}

static uint32_t timer5_read(void)
{
    return TIM5->CNT;
}

static void timer5_write(uint32_t timer_cnt)
{
    TIM5->CNT = timer_cnt;
}

vtime_t g_time_timer5 = {
    .cfg = {
        .timer_bits = 32, // 32位定时器
    },
    .ops = {
        .init = timer5_init,
        .read = timer5_read,
        .write = timer5_write,
    },
};

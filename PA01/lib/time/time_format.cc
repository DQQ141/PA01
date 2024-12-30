/**
 * @file time_format.cc
 * @author WittXie
 * @brief 时间格式化
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./time.h"

// 闰年计算
bool time_is_leap_year(uint16_t year)
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

// 获取月份的天数
uint8_t get_days_in_month(uint16_t year, uint8_t month)
{
    switch (month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31;
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        return time_is_leap_year(year) ? 29 : 28;
    default:
        return 0; // 错误处理
    }
}

// 计算日期
vdate_t timestamp_to_date(uint64_t timestamp_us)
{
    vdate_t date;
    uint64_t seconds = timestamp_us / 1000000; // 将微秒转换为秒
    timestamp_us %= 1000000;                   // 剩余的微秒

    // 计算毫秒和微秒
    date.ms = timestamp_us / 1000;
    date.us = timestamp_us % 1000;

    // 计算总秒数
    uint64_t total_seconds = seconds;

    // 从Unix时间戳开始计算日期
    date.year = 1970; // 1970年1月1日开始
    date.month = 1;
    date.day = 1;
    date.hour = 0;
    date.min = 0;
    date.sec = 0;

    // 计算年份
    while (total_seconds >= 31536000)
    { // 365天
        uint64_t year_seconds = 31536000;
        if (time_is_leap_year(date.year))
        {
            year_seconds = 31622400; // 366天
        }
        if (total_seconds >= year_seconds)
        {
            total_seconds -= year_seconds;
            date.year++;
        }
        else
        {
            break;
        }
    }

    // 计算月份和日期
    while (total_seconds >= 86400)
    { // 一天的时间
        uint8_t days_in_month = get_days_in_month(date.year, date.month);
        if (total_seconds >= days_in_month * 86400)
        {
            total_seconds -= days_in_month * 86400;
            date.month++;
            if (date.month > 12)
            {
                date.month = 1;
                date.year++;
            }
        }
        else
        {
            break;
        }
    }

    date.day += total_seconds / 86400;
    total_seconds %= 86400;

    // 计算小时、分钟和秒
    date.hour = total_seconds / 3600;
    total_seconds %= 3600;
    date.min = total_seconds / 60;
    date.sec = total_seconds % 60;

    return date;
}

// 获取编译时间
vdate_t build_date_get(void)
{
    vdate_t date = {0};
    const char time[12] = __TIME__;

    const char *const str_month_group[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    const char *const build_date = __DATE__;
    uint8_t month = 0u;
    for (uint8_t i = 0u; i < 12u; ++i)
    {
        if (memcmp(build_date, str_month_group[i], 3u) == 0u)
        {
            month = i + 1u;
            break;
        }
    }

    date.year = 2000 + atoi(build_date + 9u);
    date.month = month;
    date.day = atoi(build_date + 4u);
    date.hour = (uint8_t)atoi(time);
    date.min = (uint8_t)atoi(time + 3u);
    date.sec = (uint8_t)atoi(time + 3u);

    return date;
}

// 日期转时间戳
uint64_t date_to_timestamp(const vdate_t *date)
{
    // 计算自1970年1月1日以来的总天数
    uint64_t days = 0;
    for (uint16_t y = 1970; y < date->year; ++y)
    {
        days += time_is_leap_year(y) ? 366 : 365;
    }
    for (uint8_t m = 1; m < date->month; ++m)
    {
        days += get_days_in_month(date->year, m);
    }
    // 加上本月的天数
    days += date->day - 1;

    // 计算总秒数
    uint64_t total_seconds = days * 86400 + date->hour * 3600 + date->min * 60 + date->sec;

    // 将总秒数转换为微秒
    return total_seconds * 1000000 + date->ms * 1000 + date->us;
}

// 日期转数字
uint64_t date_to_num(const vdate_t *date)
{
    // 将日期和时间的数值相乘
    return (date->year * 10000 + date->month * 100 + date->day) * 1000000llu +
           (date->hour * 10000 + date->min * 100 + date->sec);
}

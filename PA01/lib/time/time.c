#include "./time.h"

#include "./time_format.cc"

void time_init(vtime_t *time)
{
    time->timestamp_letf = 0;
    time->ops.init();

    time->timestamp_startup = timestamp_us_get(time); // 获取启动时间
}

// 时间钩子
void time_hook(vtime_t *time)
{
    time->timestamp_letf++;
}

// 获取时间戳
uint64_t timestamp_us_get(vtime_t *time)
{
    return (time->timestamp_letf << time->cfg.timer_bits) | time->ops.read();
}

// 获取当前日期
vdate_t current_date_get(vtime_t *time)
{
    return timestamp_to_date(timestamp_us_get(time));
}

// 设置当前日期
void current_date_set(vtime_t *time, const vdate_t *date)
{
    uint64_t timestamp = date_to_timestamp(date);
    time->timestamp_letf = timestamp >> time->cfg.timer_bits;
    time->ops.write(timestamp & ((1 << time->cfg.timer_bits) - 1));
}

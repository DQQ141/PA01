#include "./time_bsp.h"

// 驱动加载
#include "./../../lib/time/time.c"

// 端口加载
#include "./port/timer.cc"

// time组
vtime_t *const g_time_group[] = {
    &g_time_timer5,
};
const uint32_t TIME_GROUP_SIZE = countof(g_time_group);

// BSP初始化
void time_bsp_init(void)
{
    for (int i = 0; i < TIME_GROUP_SIZE; i++)
    {
        time_init(g_time_group[i]);
    }
}

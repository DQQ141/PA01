#include "./bsp_base.h"

// 基础库
#include "./../lib/algorithm/fit/fit.c"   // 拟合
#include "./../lib/algorithm/sort/sort.c" // 排序
#include "./../lib/dds/dds.c"             // 数据分发
#include "./../lib/list/list.c"           // 链表
#include "./../lib/ring/ring.c"           // 环形队列
#include "./../lib/string/string.c"       // 字符串

// 滤波器
#include "./../lib/filter/lp_filter.c" // 低通滤波器

void bsp_reboot(void)
{
    NVIC_SystemReset();
}

void bsp_print_debug_mode(void)
{
    if (DEBUG)
    {
        log_info("debug: enable.");
    }
    else
    {
        log_info("debug: disable.");
    }
}

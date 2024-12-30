#include "./roller_bsp.h"

// 驱动加载
#include "./../../lib/roller/roller.c"

// 加载端口
#include "./port/encoder.cc"

// ROLLER组
roller_t *const g_roller_group[] = {
    &g_roller,
};
const uint32_t ROLLER_GROUP_SIZE = countof(g_roller_group);

// roller轮询任务
static void roller_entry(void *args)
{
    for (int i = 0; i < ROLLER_GROUP_SIZE; i++)
    {
        os_sleep(10);
        roller_init(g_roller_group[i]);
    }

    for (;;)
    {
        os_sleep(10);
        for (int i = 0; i < ROLLER_GROUP_SIZE; i++)
        {
            roller_poll(g_roller_group[i]);
        }
    }
}

// BSP初始化
void roller_bsp_init(void)
{
    os_task_create(roller_entry, "roller", NULL, OS_PRIORITY_BSP + 1, OS_TASK_STACK_MIN + 4096);
}

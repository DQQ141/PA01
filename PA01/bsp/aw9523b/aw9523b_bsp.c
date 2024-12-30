#include "./aw9523b_bsp.h"

// 驱动加载
#include "./../../lib/exio/aw9523b/aw9523b.c"

// 端口加载
#include "./port/i2c.cc"

// AW9523B组
aw9523b_t *const g_aw9523b_group[] = {
    &g_aw9523b_exio,
};
const uint32_t AW9523B_GROUP_SIZE = countof(g_aw9523b_group);

void aw9523b_entry(void *arg)
{
    for (int i = 0; i < AW9523B_GROUP_SIZE; i++)
    {
        os_sleep(10);
        aw9523b_init(g_aw9523b_group[i]);
    }

    for (;;)
    {
        for (int i = 0; i < AW9523B_GROUP_SIZE; i++)
        {
            os_sleep(100);
            aw9523b_poll(g_aw9523b_group[0]);
        }
    }
}

// BSP初始化
void aw9523b_bsp_init(void)
{
    os_task_create(aw9523b_entry, "aw9523b", NULL, OS_PRIORITY_LOWEST, OS_TASK_STACK_MIN + 1024);
}

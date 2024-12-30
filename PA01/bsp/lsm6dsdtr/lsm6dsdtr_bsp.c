#include "./lsm6dsdtr_bsp.h"

// 驱动加载
#include "./../../lib/imu/lsm6dsdtr/lsm6dsdtr.c"

// 端口加载
#include "./port/i2c.cc"

// LSM6DSDTR组
lsm6dsdtr_t *const g_lsm6dsdtr_group[] = {
    &g_lsm6dsdtr_imu,
};
const uint32_t LSM6DSDTR_GROUP_SIZE = countof(g_lsm6dsdtr_group);

static void lsm6dsdtr_entry(void *args)
{
    for (int i = 0; i < LSM6DSDTR_GROUP_SIZE; i++)
    {
        lsm6dsdtr_init(g_lsm6dsdtr_group[i]);
        os_sleep(10);
    }

    for (;;)
    {
        os_sleep(100);
        for (int i = 0; i < LSM6DSDTR_GROUP_SIZE; i++)
        {
            lsm6dsdtr_poll(g_lsm6dsdtr_group[i]);
        }
    }
}

// BSP初始化
void lsm6dsdtr_bsp_init(void)
{
    os_task_create(lsm6dsdtr_entry, "lsm6dsdtr", NULL, OS_PRIORITY_HIGHEST, OS_TASK_STACK_MIN + 1024);
}

#include "./ppm_bsp.h"

// 此驱动不能使用其它内存，只能用axi
#undef MALLOC
#undef FREE
#define MALLOC(_size) malloc(_size)

// 驱动加载
#include "./../../lib/ppm/ppm.c"

// 端口加载
#include "./port/pwm.cc"

// PPM组
ppm_t *const g_ppm_group[] = {
    &g_ppm_main,
};
const uint32_t PPM_GROUP_SIZE = countof(g_ppm_group);

static void ppm_entry(void *args)
{
    for (int i = 0; i < PPM_GROUP_SIZE; i++)
    {
        ppm_init(g_ppm_group[i]);
        os_sleep(10);
    }

    for (;;)
    {
        os_sleep(20);
        for (int i = 0; i < PPM_GROUP_SIZE; i++)
        {
            ppm_poll(g_ppm_group[i]);
        }
    }
}

// BSP初始化
void ppm_bsp_init(void)
{
    os_task_create(ppm_entry, "ppm", NULL, OS_PRIORITY_BSP, OS_TASK_STACK_MIN + 1024);

    ppm_channel_write(&g_ppm_main, 2, true);
}

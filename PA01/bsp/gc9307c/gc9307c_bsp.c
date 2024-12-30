#include "./gc9307c_bsp.h"

// 驱动加载
#include "./../../lib/screen/gc9307c/gc9307c.c"

// 端口加载
// #include "./port/gpio.cc"
#include "./port/spi_dma.cc"
// #include "./port/spi_dma_map.cc"

// GC9307C组
gc9307c_t *const g_gc9307c_group[] = {
    &g_gc9307c_lcd,
};
const uint32_t GC9307C_GROUP_SIZE = countof(g_gc9307c_group);

static void gc9307c_init_entry(void *args)
{
    for (int i = 0; i < GC9307C_GROUP_SIZE; i++)
    {
        gc9307c_init(g_gc9307c_group[i]);
    }
    log_info("gc9307c init done.");
    os_return;
}

// BSP初始化
void gc9307c_bsp_init(void)
{
    os_task_create(gc9307c_init_entry, "gc9307c_init", NULL, OS_PRIORITY_BSP, OS_TASK_STACK_MIN + 512);
}

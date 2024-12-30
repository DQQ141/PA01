#include "./log_bsp.h"

// 驱动加载
#include "./../../lib/log/log.c"

// 插件加载
#include "./../../lib/log/plugin/flush/flush.c" //缓存插件

// 端口加载
#include "./port/uart.cc"

log_t *const g_log_group[] = {
    &g_log_uart,
};
const uint32_t LOG_GROUP_SIZE = countof(g_log_group);

int fputc(int ch, FILE *f)
{
    g_log_uart.ops.write((uint8_t *)&ch, 1);
    return ch;
}

static void log_entry(void *args)
{
    for (int i = 0; i < LOG_GROUP_SIZE; i++)
    {
        log_init(g_log_group[i]);

        // 开启插件
        log_flush_init(g_log_group[i]);
    }
    log_info("log init done.");

    for (;;)
    {
        os_sleep(50);
        for (int i = 0; i < LOG_GROUP_SIZE; i++)
        {
            log_poll(g_log_group[i]);
        }
    }
}

void log_bsp_init(void)
{
    os_task_create(log_entry, "log", NULL, OS_PRIORITY_BSP + 1, OS_TASK_STACK_MIN + LOG_BUFF_SIZE);

    os_sleep_period_until(log_is_running(&g_log_uart), 10, 6000); // 等待log启动
    ASSERT(log_is_running(&g_log_uart));
}

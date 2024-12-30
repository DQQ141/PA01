#include "./protocol_bsp.h"

// 此驱动不能使用其它内存，只能用axi
#undef MALLOC
#undef FREE
#define MALLOC(_size) malloc(_size)

// #undef LOG_LEVEL
// #define LOG_LEVEL LOG_LEVEL_NONE

// 驱动加载
#include "./../../lib/protocol/protocol.c"

// 端口加载
#include "./port/uart_head.cc"
#include "./port/uart_rf.cc"
#include "./port/uart_sport.cc"
#include "./port/uart_stick.cc"
#include "./port/uart_transparent.cc"

#undef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_TRACE

// PROTOCOL组
protocol_t *const g_protocol_group[] = {
    &g_protocol_uart_stick,
    &g_protocol_uart_rf,
    &g_protocol_uart_head,
    &g_protocol_uart_sport,
};
const uint32_t PROTOCOL_GROUP_SIZE = countof(g_protocol_group);

// 协议加载
#include "./../../lib/protocol/fs_inrm303/fs_inrm303.c"
#include "./../../lib/protocol/fs_stick/fs_stick.c"

static void protocol_entry(void *args)
{
    for (int i = 0; i < PROTOCOL_GROUP_SIZE; i++)
    {
        protocol_init(g_protocol_group[i]);
    }
    transparent_init();

    // 监听
    for (;;)
    {
        for (int i = 0; i < PROTOCOL_GROUP_SIZE; i++)
        {
            protocol_poll(g_protocol_group[i]);
        }
        os_sleep(10);
    }
}

// BSP初始化
void protocol_bsp_init(void)
{
    os_task_create(protocol_entry, "protocol", NULL, OS_PRIORITY_BSP + 1, OS_TASK_STACK_MIN + 4096);
}

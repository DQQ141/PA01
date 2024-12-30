#include "./flash_bsp.h"

// 驱动加载
#include "./../../lib/flash/flash.c"

// 端口加载
#include "./port/mx25l25645g_manual.cc"
#include "./port/mx25l25645g_remap.cc"
// #include "./port/mx25_sun.cc"

// GPIO组
flash_t *const g_flash_group[] = {
    &g_flash_mx25,
};
const uint32_t FLASH_GROUP_SIZE = countof(g_flash_group);

void flash_bsp_init(void)
{
    // 手动模式
    for (int i = 0; i < countof(g_flash_group); i++)
    {
        flash_init(g_flash_group[i]);
    }
}

/**
 * @file wdg_bsp.c
 * @author WittXie
 * @brief 看门狗
 * @version 0.1
 * @date 2024-12-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./wdg_bsp.h"
static void wdg_entry(void *args)
{
    for (;;)
    {
        os_sleep(1000);
        HAL_IWDG_Refresh(&hiwdg1);
    }
}

void wdg_bsp_init(void)
{
    if (DEBUG)
    {
        return;
    }
    os_task_create(&wdg_entry, "wdg", NULL, OS_PRIORITY_IDLE, OS_TASK_STACK_MIN);
}

/**
 * @file aw9523b_test.cc
 * @author WittXie
 * @brief 测试IO拓展芯片 AW9523B
 * @version 0.1
 * @date 2024-09-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void aw9523b_entry(void *args)
{
    uint8_t cnt = 1;
    uint8_t id = 0;

    os_sleep(1000);
    for (;;)
    {
        os_sleep(500);
    }
}

static void aw9523b_test(void)
{
    os_task_create(aw9523b_entry, "aw9523b_test", NULL, OS_PRIORITY_APP, OS_TASK_STACK_MIN + 1024);
}

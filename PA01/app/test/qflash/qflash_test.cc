/**
 * @file qflash_test.cc
 * @author WittXie
 * @brief QSPI Flash 测试
 * @version 0.1
 * @date 2024-09-19
 * @note
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

#define TEST_ADDR 0x00400000
static void qflash_test(void)
{
    log_info("qflash test start.");
    print_hex8("remap 0x%08X", ((__IO uint8_t *)(0x90000000 + TEST_ADDR)), 1024, (0x90000000 + TEST_ADDR));
    log_info("flash test end.");
}

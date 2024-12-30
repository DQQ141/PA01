/**
 * @file sdram_test.cc
 * @author WittXie
 * @brief SDRAM测试
 * @version 0.1
 * @date 2024-09-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

static void sdram_test(void)
{
    log_info("sdram test start.");

    // 直接读写SDRAM
    uint32_t *sdram_test = (uint32_t *)0xC0000000;
    sdram_test[0] = 123456;
    print("direct: 0x%08X, %lu\r\n", sdram_test, sdram_test[0]);

    // 编译器link到SDRAM
    static uint32_t loop_sdram __section_sdram;
    loop_sdram = 123456;
    print("link: 0x%08X, %lu\r\n", &loop_sdram, loop_sdram);

    log_info("sdram test end.");
}

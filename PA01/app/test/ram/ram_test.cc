/**
 * @file ram_test.c
 * @author WittXie
 * @brief 内存测试
 * @version 0.1
 * @date 2024-09-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../test_app.h"

static void iram_test(void)
{
    print("iram test:\r\n");
    {
        static volatile uint32_t cnt __section_axi;
        cnt = 0;
        uint64_t time = time_spent({
            while (cnt < 1000000)
            {
                cnt++;
            }
        });
        print("uint32_t spent time: %llu us\r\n", time);
    }
    {
        static volatile uint64_t cnt __section_axi;
        cnt = 0;
        uint64_t time = time_spent({
            while (cnt < 1000000)
            {
                cnt++;
            }
        });
        print("uint64_t spent time: %llu us\r\n", time);
    }
    {
        static volatile float cnt __section_axi;
        cnt = 0;
        uint64_t time = time_spent({
            while (cnt < 1000000)
            {
                cnt++;
            }
        });
        print("float spent time: %llu us\r\n", time);
    }
    {
        static volatile double cnt __section_axi;
        cnt = 0;
        uint64_t time = time_spent({
            while (cnt < 1000000)
            {
                cnt++;
            }
        });
        print("double spent time: %llu us\r\n", time);
    }
}

static void fmc_ram_test(void)
{
    print("fmc_ram test:\r\n");
    {
        static volatile uint32_t cnt __section_sdram;
        cnt = 0;
        uint64_t time = time_spent({
            while (cnt < 1000000)
            {
                cnt++;
            }
        });
        print("uint32_t spent time: %llu us\r\n", time);
    }
    {
        static volatile uint64_t cnt __section_sdram;
        cnt = 0;
        uint64_t time = time_spent({
            while (cnt < 1000000)
            {
                cnt++;
            }
        });
        print("uint64_t spent time: %llu us\r\n", time);
    }
    {
        static volatile float cnt __section_sdram;
        cnt = 0;
        uint64_t time = time_spent({
            while (cnt < 1000000)
            {
                cnt++;
            }
        });
        print("float spent time: %llu us\r\n", time);
    }
    {
        static volatile double cnt __section_sdram;
        cnt = 0;
        uint64_t time = time_spent({
            while (cnt < 1000000)
            {
                cnt++;
            }
        });
        print("double spent time: %llu us\r\n", time);
    }
}

static void ram_test(void)
{
    fmc_ram_test();
    iram_test();
}

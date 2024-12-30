/**
 * @file delay_test.cc
 * @author WittXie
 * @brief 测试延时时间
 * @version 0.1
 * @date 2024-09-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"

#define NOP_100()        \
    {                    \
        DELAY_NOP(9, 9); \
        DELAY_NOP(0, 1); \
    }
#define NOP_1000() \
    {              \
        NOP_100(); \
        NOP_100(); \
        NOP_100(); \
        NOP_100(); \
        NOP_100(); \
        NOP_100(); \
        NOP_100(); \
        NOP_100(); \
        NOP_100(); \
        NOP_100(); \
    }
#define NOP_10000() \
    {               \
        NOP_1000(); \
        NOP_1000(); \
        NOP_1000(); \
        NOP_1000(); \
        NOP_1000(); \
        NOP_1000(); \
        NOP_1000(); \
        NOP_1000(); \
        NOP_1000(); \
        NOP_1000(); \
    }

static void delay_test(void)
{
    log_info("delay_test start");
    {
        // 100 000次NOP，计算延时时间 : NOP = 0.03462us
        uint64_t time = time_spent({
            NOP_10000();
            NOP_10000();
            NOP_10000();
            NOP_10000();
            NOP_10000();
            NOP_10000();
            NOP_10000();
            NOP_10000();
            NOP_10000();
            NOP_10000();
        });
        print("spent time: %llu us\r\n", time);
    }

    log_info("delay_test end");
}

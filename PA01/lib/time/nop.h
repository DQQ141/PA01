#define __NOP_0()
#define __NOP_0_0()
#define __NOP_1() \
    {             \
        __NOP();  \
    }
#define __NOP_2() \
    {             \
        __NOP();  \
        __NOP();  \
    }
#define __NOP_3()  \
    {              \
        __NOP_2(); \
        __NOP();   \
    }
#define __NOP_4()  \
    {              \
        __NOP_3(); \
        __NOP();   \
    }
#define __NOP_5()  \
    {              \
        __NOP_4(); \
        __NOP();   \
    }
#define __NOP_6()  \
    {              \
        __NOP_5(); \
        __NOP();   \
    }
#define __NOP_7()  \
    {              \
        __NOP_6(); \
        __NOP();   \
    }
#define __NOP_8()  \
    {              \
        __NOP_7(); \
        __NOP();   \
    }
#define __NOP_9()  \
    {              \
        __NOP_8(); \
        __NOP();   \
    }
#define __NOP_1_0() \
    {               \
        __NOP_9();  \
        __NOP();    \
    }
#define __NOP_2_0()  \
    {                \
        __NOP_1_0(); \
        __NOP_1_0(); \
    }
#define __NOP_3_0()  \
    {                \
        __NOP_2_0(); \
        __NOP_1_0(); \
    }
#define __NOP_4_0()  \
    {                \
        __NOP_2_0(); \
        __NOP_2_0(); \
    }
#define __NOP_5_0()  \
    {                \
        __NOP_4_0(); \
        __NOP_1_0(); \
    }
#define __NOP_6_0()  \
    {                \
        __NOP_3_0(); \
        __NOP_3_0(); \
    }
#define __NOP_7_0()  \
    {                \
        __NOP_6_0(); \
        __NOP_1_0(); \
    }
#define __NOP_8_0()  \
    {                \
        __NOP_4_0(); \
        __NOP_4_0(); \
    }
#define __NOP_9_0()  \
    {                \
        __NOP_5_0(); \
        __NOP_4_0(); \
    }
#define DELAY_NOP(_m, _n) \
    {                     \
        __NOP_##_m##_0(); \
        __NOP_##_n();     \
    }

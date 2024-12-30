#include "./data_bsp.h"

// 此驱动不能使用其它内存，只能用axi
#undef MALLOC
#undef FREE
#define MALLOC(_size) malloc(_size)

// 驱动
#include "./../../lib/data/data.c"

// 端口加载
#include "./port/factory.cc"

// DATA组
data_t *const g_data_group[] = {
    &g_data_factory,
};
const uint32_t DATA_GROUP_SIZE = countof(g_data_group);

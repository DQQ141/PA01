#include "./crc_bsp.h"

// 驱动加载
#include "./../../lib/encryptor/crc/crc.c"

// 校验表
#include "./table/ccitt.cc" // CCITT校验表

// CRC组
crc_t *const g_crc_group[] = {
    &g_crc_ccitt,
};
const uint32_t CRC_GROUP_SIZE = countof(g_crc_group);

// BSP初始化
void crc_bsp_init(void)
{
    for (int i = 0; i < CRC_GROUP_SIZE; i++)
    {
        if (g_crc_group[i]->table == NULL)
        {
            // 没有预先生成校验表的情况
            crc_table_create(g_crc_group[i]);
            crc_table_print(g_crc_group[i]); // 打印校验表
        }
    }
}

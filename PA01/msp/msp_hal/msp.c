#include "./msp.h"

#include "./i2c/i2c_msp.cc"
#include "./qflash/qflash_msp.cc"
#include "./sdcard/sdcard_msp.cc"
#include "./sdram/sdram_msp.cc"
#include "./spi/spi_msp.cc"
#include "./uart/uart_msp.cc"

/* 告知连接器不从C库链接使用半主机的函数 */
#pragma __use_no_semihosting

/* 定义 _sys_exit() 以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

/* 标准库需要的支持类型 */
struct __FILE
{
    int handle;
};

FILE __stdout;

void msp_first_init(void)
{
    // MX_QUADSPI_Init();
    // void flash_remap_mode(void);
    // flash_remap_mode(); // 映射模式
}

void msp_init(void)
{
    // sdram_msp_init();                          // 拓展SDRAM
    memset((void *)0xC0000000, 0, 0x00400000); // SDRAM全部初始化为0

    void os_heap_init(void);
    os_heap_init(); // 堆初始化

    sdcard_msp_init(); // SD卡初始化

    // 板级初始化
    void bsp_init(void);
    bsp_init();
}

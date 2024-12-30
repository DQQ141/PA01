/**
 * @file msp.h
 * @author WittXie
 * @brief stm32h7芯片支持包
 * @version 0.1
 * @date 2023-03-30
 * @copyright Copyright (c) 2023
 * 显示芯片RAM/ROM使用情况「 ./Keil5_disp_size_bar_v0.4.exe "./output/list/project.map" 」
 */
#pragma once

// msp
#include "./msp_env.h"

// msp_hal
#include "./gpio/gpio_msp.h"
#include "./i2c/i2c_msp.h"
#include "./spi/spi_msp.h"
#include "./uart/uart_msp.h"
const char *f_error_to_string(FRESULT res);

// rom section
#define __section_irom __attribute__((section(".irom")))
#define __section_qspi_rom __attribute__((section(".qflash_rom")))

// ram section
#define __section_dtcm __attribute__((section(".iram_dtcm")))
#define __section_axi __attribute__((section(".iram_axi")))
#define __section_sdram __attribute__((section(".bss.sdram")))
#define __section_sram1 __attribute__((section(".sram1")))
#define __section_sram2 __attribute__((section(".sram2")))
#define __section_sram3 __attribute__((section(".sram3")))
#define __section_sram4 __attribute__((section(".sram4")))

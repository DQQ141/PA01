/**
 * @file uart_msp.h
 * @author WittXie
 * @brief 芯片层SPI支持包
 * @version 0.1
 * @date 2024-05-30
 * @note timing = (PRESC << 28) | (SCLDEL << 20) | (SDADEL << 16) | (SCLH << 8) | SCLL
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "./../msp_env.h"

/**
 * @brief 获取SPI名字字符串
 *
 * @param Instance SPI
 *
 * @return const char* SPI名字字符串
 */
const char *spi_name_str_get(SPI_TypeDef *Instance);

/**
 * @brief 获取SPI错误码字符串
 *
 * @param ErrorCode 错误码
 * @return const char* SPI名字字符串
 */
const char *spi_error_str_get(uint32_t ErrorCode);
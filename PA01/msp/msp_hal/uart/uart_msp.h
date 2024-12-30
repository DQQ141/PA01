/**
 * @file uart_msp.h
 * @author WittXie
 * @brief 芯片层串口支持包
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "./../msp_env.h"

/**
 * @brief 设置串口波特率
 *
 * @param huart 串口句柄
 * @param baud_rate 波特率
 */
void uart_baud_set(UART_HandleTypeDef *huart, int baud_rate);

/**
 * @brief 中止串口
 *
 * @param huart 串口句柄
 */
void uart_abort(UART_HandleTypeDef *huart);

/**
 * @brief 获取串口名字字符串
 *
 * @param Instance 串口
 *
 * @return const char* 串口名字字符串
 */
const char *uart_name_str_get(USART_TypeDef *Instance);

/**
 * @brief 获取串口错误码字符串
 *
 * @param ErrorCode 错误码
 * @return const char* 串口名字字符串
 */
const char *uart_error_str_get(uint32_t ErrorCode);

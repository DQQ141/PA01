/**
 * @file uart.cc
 * @author WittXie
 * @brief 串口端口
 * @version 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../log_bsp.h"

#define MAKE_UART(_name, _uart, _level, _buff_size, _filter)                \
    static uint8_t __##_name##_write_buff[_buff_size] = {0};                \
    static void __##_name##_init()                                          \
    {                                                                       \
    }                                                                       \
    static void __##_name##_write(uint8_t *buff, uint32_t length)           \
    {                                                                       \
        extern UART_HandleTypeDef _uart;                                    \
        os_sleep_until(__HAL_UART_GET_FLAG(&(_uart), UART_FLAG_TC), 1000);  \
        os_sleep_until(__HAL_UART_GET_FLAG(&(_uart), UART_FLAG_TXE), 1000); \
        (_uart).gState = HAL_UART_STATE_READY;                              \
        memcpy(__##_name##_write_buff, buff, length);                       \
        HAL_UART_Transmit_DMA(&(_uart), __##_name##_write_buff, length);    \
    }                                                                       \
    log_t _name = {                                                         \
        .cfg = {                                                            \
            .name = DEFINE_TO_STR(_name),                                   \
            .level = _level,                                                \
            .buff_size = _buff_size,                                        \
            .filter = _filter,                                              \
        },                                                                  \
        .ops = {                                                            \
            .init = __##_name##_init,                                       \
            .write = __##_name##_write,                                     \
        },                                                                  \
    }

// GPIO定义
MAKE_UART(g_log_uart, huart5, LOG_LEVEL, LOG_BUFF_SIZE, NULL);

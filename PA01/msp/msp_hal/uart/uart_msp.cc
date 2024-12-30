#include "./uart_msp.h"

// 设置波特率
void uart_baud_set(UART_HandleTypeDef *huart, int baud_rate)
{
    HAL_UART_AbortReceive(huart);
    HAL_UART_AbortTransmit(huart);

    if (HAL_UART_DeInit(huart) != HAL_OK)
    {
        Error_Handler();
    }

    huart->Init.BaudRate = baud_rate;
    if (HAL_UART_Init(huart) != HAL_OK)
    {
        Error_Handler();
    }
}

void uart_abort(UART_HandleTypeDef *huart)
{
    HAL_UART_AbortReceive(huart);
    HAL_UART_AbortTransmit(huart);
}

// 获取名字字符串
const char *uart_name_str_get(USART_TypeDef *Instance)
{
    if (Instance == USART1)
    {
        return "USART1";
    }
    else if (Instance == USART2)
    {
        return "USART2";
    }
    else if (Instance == USART3)
    {
        return "USART3";
    }
    else if (Instance == UART4)
    {
        return "UART4";
    }
    else if (Instance == UART5)
    {
        return "UART5";
    }
    else if (Instance == USART6)
    {
        return "USART6";
    }
    else if (Instance == UART7)
    {
        return "UART7";
    }
    else if (Instance == UART8)
    {
        return "UART8";
    }
    else
    {
        return "Uart_Unknown";
    }
}

// 获取错误字符串
const char *uart_error_str_get(uint32_t ErrorCode)
{
    if (ErrorCode == HAL_UART_ERROR_NONE)
    {
        return "无错误: UART操作期间未发生任何错误。";
    }
    else if (ErrorCode & HAL_UART_ERROR_PE)
    {
        return "奇偶校验错误: 接收到的数据与奇偶校验位不匹配。";
    }
    else if (ErrorCode & HAL_UART_ERROR_NE)
    {
        return "噪声错误: 接收到的数据受到噪声干扰。";
    }
    else if (ErrorCode & HAL_UART_ERROR_FE)
    {
        return "帧错误: 接收到的数据帧格式不正确。";
    }
    else if (ErrorCode & HAL_UART_ERROR_ORE)
    {
        return "溢出错误: 接收缓冲区已满但仍有数据到达。";
    }
    else if (ErrorCode & HAL_UART_ERROR_DMA)
    {
        return "DMA传输错误: 发生DMA传输错误，表示与UART操作相关的DMA传输失败。";
    }
    else
    {
        return "未知错误: UART操作期间发生未知错误。";
    }
}

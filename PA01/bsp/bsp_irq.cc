/**
 * @file bsp_irq.c
 * @author WittXie
 * @brief 中断处理
 * @version 0.1
 * @date 2023-04-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "./bsp.h"

// 依赖
#include "./bsp_env.h"

// 定时器回调
void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    switch ((uint32_t)(htim->Instance))
    {
    case (uint32_t)TIM5:
        time_hook(&g_time_timer5);
        break;
    default:
        break;
    }
}

extern int32_t step;
// 外部中断回调
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // 处理外部中断
    switch (GPIO_Pin)
    {
    case GPIO_PIN_10: // PPM_INPUT
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET)
        {
            ppm_read_hook(&g_ppm_main, TIMESTAMP_US_GET());
        }
        break;
    default:
        break;
    }
}
// 串口回调
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    switch ((uint32_t)(huart->Instance))
    {
    default:
        break;
    }
}

// SPI DMA回调
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    switch ((uint32_t)hspi->Instance)
    {
    case (uint32_t)SPI1:
        break;

    default:
        break;
    }
}

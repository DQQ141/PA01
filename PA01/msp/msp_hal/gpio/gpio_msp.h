/**
 * @file gpio_msp.h
 * @author WittXie
 * @brief GPIO芯片层接口
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "./../msp_env.h"

// 定义
typedef struct __msp_gpio
{
    GPIO_TypeDef *GPIOx; // 端口
    uint16_t GPIO_Pin;   // 引脚
} msp_gpio_t;

// 接口定义
#define MSP_GPIO_READ(_io) ((bool)HAL_GPIO_ReadPin(((msp_gpio_t *)(_io))->GPIOx, ((msp_gpio_t *)(_io))->GPIO_Pin) == GPIO_PIN_SET)
#define MSP_GPIO_WRITE(_io, _bool) HAL_GPIO_WritePin(((msp_gpio_t *)(_io))->GPIOx, ((msp_gpio_t *)(_io))->GPIO_Pin, (_bool) ? GPIO_PIN_SET : GPIO_PIN_RESET);

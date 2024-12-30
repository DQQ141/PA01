/**
 * @file bsp_error.c
 * @author WittXie
 * @brief 错误异常检查/输出信息
 * @version 0.1
 * @date 2023-04-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "./bsp.h"

// 依赖
#include "./bsp_env.h"

/**
 * @brief 检查重启原因
 * RCC_FLAG_BORRST
 * RCC_FLAG_PINRST
 * RCC_FLAG_PORRST
 * RCC_FLAG_SFTRST
 * RCC_FLAG_IWDG1RST
 * RCC_FLAG_WWDG1RST
 * RCC_FLAG_LPWR1RST
 * RCC_FLAG_LPWR2RST
 */
static void reset_reason_check(void)
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
    {
        dprint(COLOR_H_GREEN "system was reset due to power-on reset\r\n");
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
    {
        dprint(COLOR_H_GREEN "system was reset due to pin reset\r\n");
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
    {
        dprint(COLOR_H_GREEN "system was reset due to power-on reset\r\n");
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        dprint(COLOR_H_GREEN "system was reset due to software reset\r\n");
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST))
    {
        dprint(COLOR_H_RED "system was reset due to IWDG1 reset\r\n");
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDG1RST))
    {
        dprint(COLOR_H_RED "system was reset due to WWDG1 reset\r\n");
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWR1RST))
    {
        dprint(COLOR_H_RED "system was reset due to illegal D1 DSTANDBY or CPU CSTOP flag\r\n");
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWR2RST))
    {
        dprint(COLOR_H_RED "system was reset due to illegal D2 DSTANDBY or CPU2 CSTOP flag\r\n");
    }
    else
    {
        dprint(COLOR_H_YELLOW "unknown reset reason\r\n");
    }

    // Clear reset flags
    __HAL_RCC_CLEAR_RESET_FLAGS(); // 清除复位标志位
}

/**
 * @brief 打印异常信息
 *
 * @param format 名称
 * @param msp 主栈指针
 * @param psp 子栈指针
 */
void back_trace(const char *name, uint32_t msp, uint32_t psp)
{
    printf(COLOR_H_RED "\r\nhardware fault occurred:%s.\r\n", name);

    // MSP 为主栈指针，用于存储主栈的地址。
    printf(COLOR_H_RED "MSP = 0x%08x\r\n", msp);                       // SP 为堆栈指针，用于存储堆栈的地址。
    printf(COLOR_H_RED "MSP->LR  = 0x%08x\r\n", ((uint32_t *)msp)[5]); // LR 为链接寄存器，用于存储函数调用后的返回地址。
    printf(COLOR_H_RED "MSP->PC  = 0x%08x\r\n", ((uint32_t *)msp)[6]); // PC 为程序计数器，用于存储将要执行的下一条指令的地址。
    printf(COLOR_H_RED "MSP->PSR = 0x%08x\r\n", ((uint32_t *)msp)[7]); // PSR 为程序状态寄存器，用于存储程序的状态信息，如条件标志位、控制位等。

    // PSP 为子栈指针，用于存储子栈的地址。
    printf(COLOR_H_RED "PSP = 0x%08x\r\n", psp);                       // SP 为堆栈指针，用于存储堆栈的地址。
    printf(COLOR_H_RED "PSP->LR  = 0x%08x\r\n", ((uint32_t *)psp)[5]); // LR 为链接寄存器，用于存储函数调用后的返回地址。
    printf(COLOR_H_RED "PSP->PC  = 0x%08x\r\n", ((uint32_t *)psp)[6]); // PC 为程序计数器，用于存储将要执行的下一条指令的地址。
    printf(COLOR_H_RED "PSP->PSR = 0x%08x\r\n", ((uint32_t *)psp)[7]); // PSR 为程序状态寄存器，用于存储程序的状态信息，如条件标志位、控制位等。

    // 打印异常信息
    printf(COLOR_H_RED "SCB->HFSR = 0x%08X\r\n", SCB->HFSR);  // HFSR（HardFault Status Register）：用于记录硬件故障的状态信息。
    printf(COLOR_H_RED "SCB->CFSR = 0x%08X\r\n", SCB->CFSR);  // CFSR（Configurable Fault Status Register）：用于记录配置错误的状态信息。
    printf(COLOR_H_RED "SCB->MMFAR= 0x%08X\r\n", SCB->MMFAR); // MMFAR（MemManage Fault Address Register）：用于记录最近一次内存管理错误的地址。
    printf(COLOR_H_RED "SCB->BFAR = 0x%08X\r\n", SCB->BFAR);  // BFAR（BusFault Address Register）：用于记录最近一次总线错误的地址。
    printf(COLOR_H_RED "SCB->AFSR = 0x%08X\r\n", SCB->AFSR);  // AFSR（Auxiliary Fault Status Register）：用于记录辅助故障的状态信息。
    printf(COLOR_H_RED "SCB->SHCSR= 0x%08X\r\n", SCB->SHCSR); // SHCSR（SVC Hard Fault Status Register）：用于记录SVC调用的硬件故障的状态信息。
    printf(COLOR_H_RED "SCB->CPACR= 0x%08X\r\n", SCB->CPACR); // CPACR（Coprocessor Access Control Register）：用于控制访问权限
    printf(COLOR_H_RED "SCB->CPACR= 0x%08X\r\n", SCB->CPACR); // CPACR （Coprocessor Access Control Register）：用于控制访问权限

    printf(COLOR_H_RED "\r\n\n\n");
}

/**
 * @brief 处理器运行时产生错误
 *
 * @param file 文件路径
 * @param line 行号
 */
void error_from(char *file, uint32_t line)
{
    printf(COLOR_H_RED "\r\nassert failed occurred %s:%u\r\n", path_remove(file), line);
}

// HAL错误钩子
void hal_error_hook(void)
{
    // 打印错误信息
    printf("hal error occurred.\r\n");
}

// 串口错误中断回调
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    // 处理UART错误
    __HAL_UART_CLEAR_PEFLAG(huart);
    __HAL_UART_CLEAR_FEFLAG(huart);
    __HAL_UART_CLEAR_NEFLAG(huart);
    __HAL_UART_CLEAR_OREFLAG(huart);

    // 打印错误信息
    // printf(COLOR_H_RED "\r\nuart error occurred:0x%08X[%s] 0x%02X[%s].\r\n",
    //        (uint32_t)huart->Instance, uart_name_str_get(huart->Instance),
    //        (uint32_t)huart->ErrorCode, uart_error_str_get(huart->ErrorCode));

    // uart_baud_set(huart, huart->Init.BaudRate);
    // HAL_UARTEx_ReceiveToIdle_DMA(huart, huart->pRxBuffPtr, huart->RxXferSize);
}

// SPI出错回调
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    printf(COLOR_H_RED "\r\nspi error occurred:0x%08X[%s] 0x%02X[%s].\r\n",
           (uint32_t)hspi->Instance, spi_name_str_get(hspi->Instance),
           (uint32_t)hspi->ErrorCode, spi_error_str_get(hspi->ErrorCode));
}

// I2C出错回调
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    printf(COLOR_H_RED "\r\ni2c error occurred:0x%08X[%s] 0x%02X[%s].\r\n",
           (uint32_t)hi2c->Instance, i2c_name_str_get(hi2c->Instance),
           (uint32_t)hi2c->ErrorCode, i2c_error_str_get(hi2c->ErrorCode));
}

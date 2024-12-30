/**
 * @file mx25_sun.cc
 * @author Sun
 * @brief mx25l25645g QSPI驱动
 * @version 0.1
 * @date 2024-12-19
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "./../flash_bsp.h"
#include "./qspi.cc"

#define QSPI_W25Qxx_OK 0             // W25Qxx通信正常
#define W25Qxx_ERROR_INIT -1         // 初始化错误
#define W25Qxx_ERROR_WriteEnable -2  // 写使能错误
#define W25Qxx_ERROR_AUTOPOLLING -3  // 轮询等待错误，无响应
#define W25Qxx_ERROR_Erase -4        // 擦除错误
#define W25Qxx_ERROR_TRANSMIT -5     // 传输错误
#define W25Qxx_ERROR_MemoryMapped -6 // 内存映射模式错误

#define W25Qxx_CMD_EnableReset 0x66 // 使能复位
#define W25Qxx_CMD_ResetDevice 0x99 // 复位器件
#define W25Qxx_CMD_JedecID 0x9F     // JEDEC ID
#define W25Qxx_CMD_WriteEnable 0X06 // 写使能

#define W25Qxx_CMD_READQSPIID 0xAF

#define W25Qxx_CMD_Enter4B 0XB7 // enter 4 address mode
#define W25Qxx_CMD_Exit4B 0XE9  // default 3 address mode

#define W25Qxx_CMD_EnterQPI 0X35 // enter 4 address mode
#define W25Qxx_CMD_ExitQPI 0XF5  // default 3 address mode

#define W25Qxx_CMD_SectorErase 0x21    // 扇区擦除，4K字节， 参考擦除时间 45ms
#define W25Qxx_CMD_BlockErase_32K 0x5C // 块擦除，  32K字节，参考擦除时间 120ms
#define W25Qxx_CMD_BlockErase_64K 0xDC // 块擦除，  64K字节，参考擦除时间 150ms
#define W25Qxx_CMD_ChipErase 0x60      // 整片擦除，参考擦除时间 20S

#define W25Qxx_CMD_QuadInputPageProgram 0x3E // 1-1-4模式下(1线指令1线地址4线数据)，页编程指令，参考写入时间 0.4ms
#define W25Qxx_CMD_FastReadQuad_IO 0x0C      // 1-4-4模式下(1线指令4线地址4线数据)，快速读取指令

#define W25Qxx_CMD_ReadStatus_REG1 0x05 // 读状态寄存器1
#define W25Qxx_Status_REG1_BUSY 0x01    // 读状态寄存器1的第0位（只读），Busy标志位，当正在擦除/写入数据/写命令时会被置1
#define W25Qxx_Status_REG1_WEL 0x02     // 读状态寄存器1的第1位（只读），WEL写使能标志位，该标志位为1时，代表可以进行写操作

#define W25Qxx_CMD_ReadConfig_REG1 0x15 // 读状态寄存器1
#define W25Qxx_CONFIG_REG_4BYTE 0x20    // 读状态寄存器1的第0位（只读），Busy标志位，当正在擦除/写入数据/写命令时会被置1
#define W25Qxx_CONFIG_REG_QPI 0x40      // 读状态寄存器1的第0位（只读），Busy标志位，当正在擦除/写入数据/写命令时会被置1

#define W25Qxx_PageSize 256                  // 页大小，256字节
#define W25Qxx_FlashSize 0x2000000           // W25Q64大小，8M字节
#define W25Qxx_FLASH_ID 0xC22019             // W25Q64 JEDEC ID
#define W25Qxx_ChipErase_TIMEOUT_MAX 100000U // 超时等待时间，W25Q64整片擦除所需最大时间是100S
#define W25Qxx_MEM_ADDR 0x90000000           // 内存映射模式的地址

/*----------------------------------------------- 函数声明 ---------------------------------------------------*/

int8_t QSPI_W25Qxx_Init(void);             // W25Qxx初始化
int8_t QSPI_W25Qxx_Reset(void);            // 复位器件
uint32_t QSPI_W25Qxx_ReadID(void);         // 读取器件ID
int8_t QSPI_W25Qxx_MemoryMappedMode(void); // 进入内存映射模式

int8_t QSPI_W25Qxx_SectorErase(uint32_t SectorAddress);    // 扇区擦除，4K字节， 参考擦除时间 45ms
int8_t QSPI_W25Qxx_BlockErase_32K(uint32_t SectorAddress); // 块擦除，  32K字节，参考擦除时间 120ms
int8_t QSPI_W25Qxx_BlockErase_64K(uint32_t SectorAddress); // 块擦除，  64K字节，参考擦除时间 150ms，实际使用建议使用64K擦除，擦除的时间最快
int8_t QSPI_W25Qxx_ChipErase(void);                        // 整片擦除，参考擦除时间 20S

int8_t QSPI_W25Qxx_WritePage(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite); // 按页写入，最大256字节
int8_t QSPI_W25Qxx_WriteBuffer(uint8_t *pData, uint32_t WriteAddr, uint32_t Size);           // 写入数据，最大不能超过flash芯片的大小
int8_t QSPI_W25Qxx_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);  // 读取数据，最大不能超过flash芯片的大小

int8_t QSPI_W25Qxx_Enter4byteMode(void);
int8_t QSPI_W25Qxx_Exit4byteMode(void);
int8_t QSPI_W25Qxx_EnterQSPIMode(void);
int8_t QSPI_W25Qxx_ExitQSPIMode(void);
uint8_t QSPI_W25Qxx_StatusR(void);

// W25Qxx初始化函数
int8_t QSPI_W25Qxx_Init(void)
{
    uint32_t Device_ID;
    QSPI_W25Qxx_Reset(); // 重置W25Qxx
    QSPI_W25Qxx_Enter4byteMode();
    Device_ID = QSPI_W25Qxx_ReadID();            // 读取设备ID
    log_info("MX25_ID = 0x%08X\r\n", Device_ID); // 打印错误信息

    if (Device_ID == 0x00C22019)
    {
        return QSPI_W25Qxx_OK; // 初始化成功
    }
    else
    {
        return W25Qxx_ERROR_INIT; // 初始化失败
    }
}

int8_t QSPI_W25Qxx_Enter4byteMode(void)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_Enter4B;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Write enable command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }

    s_config.Match = 0x20;
    s_config.Mask = W25Qxx_CONFIG_REG_4BYTE;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = W25Qxx_CMD_ReadConfig_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return QSPI_W25Qxx_OK; // 写使能成功
}

int8_t QSPI_W25Qxx_Exit4byteMode(void)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_Exit4B;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Write enable command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }

    s_config.Match = 0x00;
    s_config.Mask = W25Qxx_CONFIG_REG_4BYTE;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = W25Qxx_CMD_ReadConfig_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return QSPI_W25Qxx_OK; // 写使能成功
}

int8_t QSPI_W25Qxx_EnterQSPIMode(void)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_EnterQPI;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Write enable command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }

    s_config.Match = 0x40;
    s_config.Mask = W25Qxx_CONFIG_REG_QPI;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = W25Qxx_CMD_ReadStatus_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return QSPI_W25Qxx_OK; // 写使能成功
}

int8_t QSPI_W25Qxx_ExitQSPIMode(void)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_ExitQPI;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Write enable command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }

    s_config.Match = 0x00;
    s_config.Mask = W25Qxx_CONFIG_REG_4BYTE;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = W25Qxx_CMD_ReadStatus_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return QSPI_W25Qxx_OK; // 写使能成功
}

// 自动轮询内存就绪状态
int8_t QSPI_W25Qxx_AutoPollingMemReady(void)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_ReadStatus_REG1;

    s_config.Match = 0;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    s_config.StatusBytesSize = 1;
    s_config.Mask = W25Qxx_Status_REG1_BUSY | W25Qxx_Status_REG1_WEL;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx AutoPolling Error: Memory not ready\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return QSPI_W25Qxx_OK; // 自动轮询成功
}

// W25Qxx重置函数
int8_t QSPI_W25Qxx_Reset(void)
{
    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_EnableReset;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx Reset Error: Enable reset failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_INIT; // 重置失败
    }

    if (QSPI_W25Qxx_AutoPollingMemReady() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx Reset Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }

    s_command.Instruction = W25Qxx_CMD_ResetDevice;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx Reset Error: Reset device failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_INIT; // 重置失败
    }
    if (QSPI_W25Qxx_AutoPollingMemReady() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx Reset Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    SLEEP_MS(10);          // 延时10ms
    return QSPI_W25Qxx_OK; // 重置成功
}

// 读取W25Qxx设备ID
uint32_t QSPI_W25Qxx_ReadID(void)
{
    QSPI_CommandTypeDef s_command;
    uint8_t QSPI_ReceiveBuff[3];
    uint32_t W25Qxx_ID;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 3;
    s_command.Instruction = W25Qxx_CMD_JedecID;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx ReadID Error: Command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_INIT; // 读取ID失败
    }
    if (HAL_QSPI_Receive(&hqspi, QSPI_ReceiveBuff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx ReadID Error: Receive failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_TRANSMIT; // 接收数据失败
    }
    W25Qxx_ID = (QSPI_ReceiveBuff[0] << 16) | (QSPI_ReceiveBuff[1] << 8) | QSPI_ReceiveBuff[2];

    return W25Qxx_ID; // 返回设备ID
}

uint8_t QSPI_W25Qxx_StatusR(void)
{
    QSPI_CommandTypeDef s_command;
    uint8_t QSPI_ReceiveBuff[1];
    uint8_t StatusR;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 1;
    s_command.Instruction = W25Qxx_CMD_ReadStatus_REG1;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx ReadID Error: Command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_INIT; // 读取ID失败
    }
    if (HAL_QSPI_Receive(&hqspi, QSPI_ReceiveBuff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx ReadID Error: Receive failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_TRANSMIT; // 接收数据失败
    }
    StatusR = QSPI_ReceiveBuff[0];

    return StatusR; // 返回设备ID
}

// 进入内存映射模式
int8_t QSPI_W25Qxx_MemoryMappedMode(void)
{
    QSPI_CommandTypeDef s_command;
    QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

    s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_4_LINES;
    s_command.DataMode = QSPI_DATA_4_LINES;
    s_command.DummyCycles = 6;
    s_command.Instruction = W25Qxx_CMD_FastReadQuad_IO;

    s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    s_mem_mapped_cfg.TimeOutPeriod = 0;

    QSPI_W25Qxx_Reset(); // 重置W25Qxx

    if (QSPI_MemoryMapped(&hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK)
    {
        // log_error("W25Qxx MemoryMapped Error: Memory mapped mode failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_MemoryMapped; // 内存映射模式失败
    }

    return QSPI_W25Qxx_OK; // 内存映射模式成功
}

// 写使能
int8_t QSPI_W25Qxx_WriteEnable(void)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_WriteEnable;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Write enable command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }

    s_config.Match = 0x02;
    s_config.Mask = W25Qxx_Status_REG1_WEL;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = W25Qxx_CMD_ReadStatus_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WriteEnable Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return QSPI_W25Qxx_OK; // 写使能成功
}

// 扇区擦除
int8_t QSPI_W25Qxx_SectorErase(uint32_t SectorAddress)
{
    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Address = SectorAddress;
    s_command.Instruction = W25Qxx_CMD_SectorErase;

    if (QSPI_W25Qxx_WriteEnable() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx SectorErase Error: Write enable failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx SectorErase Error: Erase command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_Erase; // 擦除失败
    }
    if (QSPI_W25Qxx_AutoPollingMemReady() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx SectorErase Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return QSPI_W25Qxx_OK; // 扇区擦除成功
}

// 32KB块擦除
int8_t QSPI_W25Qxx_BlockErase_32K(uint32_t SectorAddress)
{
    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Address = SectorAddress;
    s_command.Instruction = W25Qxx_CMD_BlockErase_32K;

    if (QSPI_W25Qxx_WriteEnable() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx BlockErase_32K Error: Write enable failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx BlockErase_32K Error: Erase command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_Erase; // 擦除失败
    }
    if (QSPI_W25Qxx_AutoPollingMemReady() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx BlockErase_32K Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return QSPI_W25Qxx_OK; // 32KB块擦除成功
}

// 64KB块擦除
int8_t QSPI_W25Qxx_BlockErase_64K(uint32_t SectorAddress)
{
    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Address = SectorAddress;
    s_command.Instruction = W25Qxx_CMD_BlockErase_64K;

    if (QSPI_W25Qxx_WriteEnable() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx BlockErase_64K Error: Write enable failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE * 2) != HAL_OK)
    {
        // log_error("W25Qxx BlockErase_64K Error: Erase command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_Erase; // 擦除失败
    }

    QSPI_AutoPollingTypeDef s_config;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_ReadStatus_REG1;

    s_config.Match = 0;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    s_config.StatusBytesSize = 1;
    s_config.Mask = W25Qxx_Status_REG1_BUSY | W25Qxx_Status_REG1_WEL;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE * 100) != HAL_OK)
    {
        // log_error("W25Qxx AutoPolling Error: Memory not ready\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }

    return QSPI_W25Qxx_OK; // 64KB块擦除成功
}

// 芯片擦除
int8_t QSPI_W25Qxx_ChipErase(void)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_ChipErase;

    // QSPI_W25Qxx_Exit4byteMode();

    if (QSPI_W25Qxx_WriteEnable() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx ChipErase Error: Write enable failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx ChipErase Error: Erase command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_Erase; // 擦除失败
    }

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.Instruction = W25Qxx_CMD_ReadStatus_REG1;

    s_config.Match = 0;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    s_config.StatusBytesSize = 1;
    s_config.Mask = W25Qxx_Status_REG1_BUSY | W25Qxx_Status_REG1_WEL;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE * 120) != HAL_OK)
    {
        // log_error("W25Qxx AutoPolling Error: Memory not ready\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }

    // QSPI_W25Qxx_Enter4byteMode();
    return QSPI_W25Qxx_OK; // 芯片擦除成功
}

// 写入一页数据
int8_t QSPI_W25Qxx_WritePage(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = NumByteToWrite;
    s_command.Address = WriteAddr;
    s_command.Instruction = 0x12; // W25Qxx_CMD_QuadInputPageProgram;

    if (QSPI_W25Qxx_WriteEnable() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx WritePage Error: Write enable failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_WriteEnable; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WritePage Error: Command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_TRANSMIT; // 命令发送失败
    }
    if (HAL_QSPI_Transmit(&hqspi, pBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx WritePage Error: Transmit failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_TRANSMIT; // 数据发送失败
    }
    if (QSPI_W25Qxx_AutoPollingMemReady() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx WritePage Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }

    return QSPI_W25Qxx_OK; // 写入一页数据成功
}

// 写入缓冲区数据
int8_t QSPI_W25Qxx_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t Size)
{
    uint32_t end_addr, current_size, current_addr;
    uint8_t *write_data;

    current_size = W25Qxx_PageSize - (WriteAddr % W25Qxx_PageSize);

    if (current_size > Size)
    {
        current_size = Size;
    }

    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;
    write_data = pBuffer;

    do
    {
        if (QSPI_W25Qxx_WriteEnable() != QSPI_W25Qxx_OK)
        {
            // log_error("W25Qxx WriteBuffer Error: Write enable failed\r\n"); // 打印错误信息
            return W25Qxx_ERROR_WriteEnable; // 写使能失败
        }
        else if (QSPI_W25Qxx_WritePage(write_data, current_addr, current_size) != QSPI_W25Qxx_OK)
        {
            // log_error("W25Qxx WriteBuffer Error: Write page failed\r\n"); // 打印错误信息
            return W25Qxx_ERROR_TRANSMIT; // 写入页数据失败
        }
        else if (QSPI_W25Qxx_AutoPollingMemReady() != QSPI_W25Qxx_OK)
        {
            // log_error("W25Qxx WriteBuffer Error: Auto polling failed\r\n"); // 打印错误信息
            return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
        }
        else
        {
            current_addr += current_size;
            write_data += current_size;
            current_size = ((current_addr + W25Qxx_PageSize) > end_addr) ? (end_addr - current_addr) : W25Qxx_PageSize;
        }
    } while (current_addr < end_addr);

    return QSPI_W25Qxx_OK; // 写入缓冲区数据成功
}

// 读取缓冲区数据
int8_t QSPI_W25Qxx_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = NumByteToRead;
    s_command.Address = ReadAddr;
    s_command.Instruction = 0x13; // W25Qxx_CMD_FastReadQuad_IO;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx ReadBuffer Error: Command failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_TRANSMIT; // 命令发送失败
    }

    if (HAL_QSPI_Receive(&hqspi, pBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // log_error("W25Qxx ReadBuffer Error: Receive failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_TRANSMIT; // 接收数据失败
    }

    if (QSPI_W25Qxx_AutoPollingMemReady() != QSPI_W25Qxx_OK)
    {
        // log_error("W25Qxx ReadBuffer Error: Auto polling failed\r\n"); // 打印错误信息
        return W25Qxx_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return QSPI_W25Qxx_OK; // 读取缓冲区数据成功
}

#define MX25L25645G_PAGE_SIZE (256)
#define MX25L25645G_SECTOR_SIZE (4 * 1024)
#define MX25L25645G_BLOCK_SIZE (64 * 1024)
#define MX25L25645G_CHIP_SIZE (32 * 1024 * 1024)
static void mx25_manual_mode_init(void)
{
    ASSERT(QSPI_W25Qxx_Init() == QSPI_W25Qxx_OK);
}

static void mx25_read(uint32_t address, uint8_t *buff, uint32_t length)
{
    ASSERT(QSPI_W25Qxx_ReadBuffer(buff, address, length) == QSPI_W25Qxx_OK);
}

static void mx25_write(uint32_t address, uint8_t *buff, uint32_t length)
{

    ASSERT(QSPI_W25Qxx_WriteBuffer(buff, address, length) == QSPI_W25Qxx_OK);
}

static void mx25_sector_erase(uint32_t address)
{
    ASSERT(QSPI_W25Qxx_SectorErase(address) == QSPI_W25Qxx_OK);
}

static void mx25_sector_erase64k(uint32_t address)
{
    ASSERT(QSPI_W25Qxx_BlockErase_64K(address) == QSPI_W25Qxx_OK);
}

flash_t g_flash_mx25 = {
    .cfg = {
        .name = "g_flash_mx25",
        .write_max_size = MX25L25645G_PAGE_SIZE,
        .erase_min_size = MX25L25645G_SECTOR_SIZE,
        .erase_block_size = MX25L25645G_BLOCK_SIZE,
        .chip_size = MX25L25645G_CHIP_SIZE,
    },
    .ops = {
        .init = &mx25_manual_mode_init, // 只有手动模式才能使用这些接口
        .read = &mx25_read,
        .write = &mx25_write,
        .erase = &mx25_sector_erase,
        .erase_block = &mx25_sector_erase64k,
    },
};

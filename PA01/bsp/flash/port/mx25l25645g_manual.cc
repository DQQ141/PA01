/**
 * @file manual_mx25l25645g_manual.cc
 * @author Sun, Xie
 * @brief mx25l25645g 手动模式
 * @version 0.1
 * @date 2024-09-22
 * @note
 * PB2	QUADSPI_CLK
 * PB6	QUADSPI_BK1_NCS
 * PD11	QUADSPI_BK1_IO0
 * PD12	QUADSPI_BK1_IO1
 * PD13	QUADSPI_BK1_IO3
 * PE2	QUADSPI_BK1_IO2
 * @copyright Copyright (c) 2024
 *
 */
#include "./../flash_bsp.h"

#define MX25L25645G_PAGE_SIZE (256)
#define MX25L25645G_SECTOR_SIZE (4 * 1024)
#define MX25L25645G_BLOCK_SIZE (64 * 1024)
#define MX25L25645G_CHIP_SIZE (32 * 1024 * 1024)
#define MX25L25645G_ID 0xC22019
#define MX25L25645G_BASE_ADDRESS 0x90000000

#define MX25L25645G_OK 0                  // mx25l25645g通信正常
#define MX25L25645G_ERROR_INIT -1         // 初始化错误
#define MX25L25645G_ERROR_WRITEENABLE -2  // 写使能错误
#define MX25L25645G_ERROR_AUTOPOLLING -3  // 轮询等待错误，无响应
#define MX25L25645G_ERROR_ERASE -4        // 擦除错误
#define MX25L25645G_ERROR_TRANSMIT -5     // 传输错误
#define MX25L25645G_ERROR_MEMORYMAPPED -6 // 内存映射模式错误

#define MX25L25645G_CMD_ENABLERESET 0x66 // 使能复位
#define MX25L25645G_CMD_RESETDEVICE 0x99 // 复位器件
#define MX25L25645G_CMD_JEDECID 0x9F     // JEDEC ID
#define MX25L25645G_CMD_WRITEENABLE 0x06 // 写使能

#define MX25L25645G_CMD_READQSPIID 0xAF // 读取QSPI ID

#define MX25L25645G_CMD_ENTER4B 0xB7 // 进入4地址模式
#define MX25L25645G_CMD_EXIT4B 0xE9  // 退出4地址模式（默认3地址模式）

#define MX25L25645G_CMD_ENTERQPI 0x35 // 进入QPI模式
#define MX25L25645G_CMD_EXITQPI 0xF5  // 退出QPI模式（默认3地址模式）

#define MX25L25645G_CMD_SECTOERASE 0x21     // 扇区擦除，4K字节，参考擦除时间 45ms
#define MX25L25645G_CMD_BLOCKERASE_32K 0x5C // 块擦除，32K字节，参考擦除时间 120ms
#define MX25L25645G_CMD_BLOCKERASE_64K 0xDC // 块擦除，64K字节，参考擦除时间 150ms
#define MX25L25645G_CMD_CHIPERASE 0x60      // 整片擦除，参考擦除时间 20S

#define MX25L25645G_CMD_QUADINPUTPAGEPROGRAM 0x12
#define MX25L25645G_CMD_FASTREADQUAD_IO 0x0C
#define MX25L25645G_CMD_READ 0x13

#define MX25L25645G_CMD_READSTATUS_REG1 0x05 // 读状态寄存器1
#define MX25L25645G_STATUS_REG1_BUSY 0x01    // 读状态寄存器1的第0位（只读），Busy标志位，当正在擦除/写入数据/写命令时会被置1
#define MX25L25645G_STATUS_REG1_WEL 0x02     // 读状态寄存器1的第1位（只读），WEL写使能标志位，该标志位为1时，代表可以进行写操作

#define MX25L25645G_CMD_READCONFIG_REG1 0x15 // 读配置寄存器1
#define MX25L25645G_CONFIG_REG_4BYTE 0x20    // 配置寄存器1的第5位，表示4字节地址模式
#define MX25L25645G_CONFIG_REG_QPI 0x40      // 配置寄存器1的第6位，表示QPI模式

#undef HAL_QPSI_TIMEOUT_DEFAULT_VALUE
#define HAL_QPSI_TIMEOUT_DEFAULT_VALUE 100000 // 默认超时时间 100s

// 函数实现
static int8_t manual_mx25l25645g_manual_mode_enter(void)
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
    s_command.Instruction = MX25L25645G_CMD_ENTER4B;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }

    s_config.Match = 0x20;
    s_config.Mask = MX25L25645G_CONFIG_REG_4BYTE;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = MX25L25645G_CMD_READCONFIG_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return MX25L25645G_OK; // 写使能成功
}

static int8_t manual_mx25l25645g_manual_mode_exit(void)
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
    s_command.Instruction = MX25L25645G_CMD_EXIT4B;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }

    s_config.Match = 0x00;
    s_config.Mask = MX25L25645G_CONFIG_REG_4BYTE;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = MX25L25645G_CMD_READCONFIG_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return MX25L25645G_OK; // 写使能成功
}

static int8_t manual_mx25l25645g_enterqpimode(void)
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
    s_command.Instruction = MX25L25645G_CMD_ENTERQPI;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }

    s_config.Match = 0x40;
    s_config.Mask = MX25L25645G_CONFIG_REG_QPI;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = MX25L25645G_CMD_READSTATUS_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return MX25L25645G_OK; // 写使能成功
}

static int8_t manual_mx25l25645g_exitqpimode(void)
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
    s_command.Instruction = MX25L25645G_CMD_EXITQPI;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }

    s_config.Match = 0x00;
    s_config.Mask = MX25L25645G_CONFIG_REG_4BYTE;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = MX25L25645G_CMD_READSTATUS_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return MX25L25645G_OK; // 写使能成功
}

static int8_t manual_mx25l25645g_autopollinmemready(void)
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
    s_command.Instruction = MX25L25645G_CMD_READSTATUS_REG1;

    s_config.Match = 0;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    s_config.StatusBytesSize = 1;
    s_config.Mask = MX25L25645G_STATUS_REG1_BUSY | MX25L25645G_STATUS_REG1_WEL;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return MX25L25645G_OK; // 自动轮询成功
}

static int8_t manual_mx25l25645g_reset(void)
{
    HAL_QSPI_DeInit(&hqspi);
    HAL_QSPI_Init(&hqspi);

    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Instruction = MX25L25645G_CMD_ENABLERESET;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_INIT; // 重置失败
    }

    if (manual_mx25l25645g_autopollinmemready() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }

    s_command.Instruction = MX25L25645G_CMD_RESETDEVICE;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_INIT; // 重置失败
    }
    if (manual_mx25l25645g_autopollinmemready() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    SLEEP_MS(10);          // 延时10ms
    return MX25L25645G_OK; // 重置成功
}

static uint32_t manual_mx25l25645g_id_read(void)
{
    QSPI_CommandTypeDef s_command;
    uint8_t QSPI_ReceiveBuff[3];
    uint32_t manual_mx25l25645g_id;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 3;
    s_command.Instruction = MX25L25645G_CMD_JEDECID;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_INIT; // 读取ID失败
    }
    if (HAL_QSPI_Receive(&hqspi, QSPI_ReceiveBuff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_TRANSMIT; // 接收数据失败
    }
    manual_mx25l25645g_id = (QSPI_ReceiveBuff[0] << 16) | (QSPI_ReceiveBuff[1] << 8) | QSPI_ReceiveBuff[2];

    return manual_mx25l25645g_id; // 返回设备ID
}

static uint8_t manual_mx25l25645g_statusr(void)
{
    QSPI_CommandTypeDef s_command;
    uint8_t QSPI_ReceiveBuff[1];
    uint8_t statusr;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 1;
    s_command.Instruction = MX25L25645G_CMD_READSTATUS_REG1;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_INIT; // 读取ID失败
    }
    if (HAL_QSPI_Receive(&hqspi, QSPI_ReceiveBuff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_TRANSMIT; // 接收数据失败
    }
    statusr = QSPI_ReceiveBuff[0];

    return statusr; // 返回设备ID
}

static int8_t manual_mx25l25645g_writeenable(void)
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
    s_command.Instruction = MX25L25645G_CMD_WRITEENABLE;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }

    s_config.Match = 0x02;
    s_config.Mask = MX25L25645G_STATUS_REG1_WEL;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = MX25L25645G_CMD_READSTATUS_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return MX25L25645G_OK; // 写使能成功
}

static int8_t manual_mx25l25645g_sector_erase(uint32_t address)
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
    s_command.Address = address;
    s_command.Instruction = MX25L25645G_CMD_SECTOERASE;

    if (manual_mx25l25645g_writeenable() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_ERASE; // 擦除失败
    }
    if (manual_mx25l25645g_autopollinmemready() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return MX25L25645G_OK; // 扇区擦除成功
}

static int8_t manual_mx25l25645g_blockerase_32k(uint32_t address)
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
    s_command.Address = address;
    s_command.Instruction = MX25L25645G_CMD_BLOCKERASE_32K;

    if (manual_mx25l25645g_writeenable() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_ERASE; // 擦除失败
    }
    if (manual_mx25l25645g_autopollinmemready() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return MX25L25645G_OK; // 32KB块擦除成功
}

static int8_t manual_mx25l25645g_blockerase_64k(uint32_t address)
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
    s_command.Address = address;
    s_command.Instruction = MX25L25645G_CMD_BLOCKERASE_64K;

    if (manual_mx25l25645g_writeenable() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE * 2) != HAL_OK)
    {
        return MX25L25645G_ERROR_ERASE; // 擦除失败
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
    s_command.Instruction = MX25L25645G_CMD_READSTATUS_REG1;

    s_config.Match = 0;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    s_config.StatusBytesSize = 1;
    s_config.Mask = MX25L25645G_STATUS_REG1_BUSY | MX25L25645G_STATUS_REG1_WEL;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE * 100) != HAL_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }

    return MX25L25645G_OK; // 64KB块擦除成功
}

static int8_t manual_mx25l25645g_chiperase(void)
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
    s_command.Instruction = MX25L25645G_CMD_CHIPERASE;

    if (manual_mx25l25645g_writeenable() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_ERASE; // 擦除失败
    }

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.Instruction = MX25L25645G_CMD_READSTATUS_REG1;

    s_config.Match = 0;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    s_config.StatusBytesSize = 1;
    s_config.Mask = MX25L25645G_STATUS_REG1_BUSY | MX25L25645G_STATUS_REG1_WEL;

    if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE * 120) != HAL_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }

    return MX25L25645G_OK; // 芯片擦除成功
}

static int8_t manual_mx25l25645g_writepage(uint8_t *buff, uint32_t address, uint16_t numbytetowrite)
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
    s_command.NbData = numbytetowrite;
    s_command.Address = address;
    s_command.Instruction = MX25L25645G_CMD_QUADINPUTPAGEPROGRAM;

    if (manual_mx25l25645g_writeenable() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
    }
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_TRANSMIT; // 命令发送失败
    }
    if (HAL_QSPI_Transmit(&hqspi, buff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_TRANSMIT; // 数据发送失败
    }
    if (manual_mx25l25645g_autopollinmemready() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }

    return MX25L25645G_OK; // 写入一页数据成功
}

static int8_t manual_mx25l25645g_write(uint32_t address, uint8_t *buff, uint32_t length)
{

    uint32_t end_addr, current_size, current_addr;
    uint8_t *write_data;

    current_size = MX25L25645G_PAGE_SIZE - (address % MX25L25645G_PAGE_SIZE);

    if (current_size > length)
    {
        current_size = length;
    }

    current_addr = address;
    end_addr = address + length;
    write_data = buff;

    do
    {
        if (manual_mx25l25645g_writeenable() != MX25L25645G_OK)
        {
            return MX25L25645G_ERROR_WRITEENABLE; // 写使能失败
        }
        else if (manual_mx25l25645g_writepage(write_data, current_addr, current_size) != MX25L25645G_OK)
        {
            return MX25L25645G_ERROR_TRANSMIT; // 写入页数据失败
        }
        else if (manual_mx25l25645g_autopollinmemready() != MX25L25645G_OK)
        {
            return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
        }
        else
        {
            current_addr += current_size;
            write_data += current_size;
            current_size = ((current_addr + MX25L25645G_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : MX25L25645G_PAGE_SIZE;
        }
    } while (current_addr < end_addr);

    return MX25L25645G_OK; // 写入缓冲区数据成功
}

static int8_t manual_mx25l25645g_read(uint32_t address, uint8_t *buff, uint32_t length)
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
    s_command.NbData = length;
    s_command.Address = address;
    s_command.Instruction = MX25L25645G_CMD_READ;

    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_TRANSMIT; // 命令发送失败
    }

    if (HAL_QSPI_Receive(&hqspi, buff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return MX25L25645G_ERROR_TRANSMIT; // 接收数据失败
    }

    if (manual_mx25l25645g_autopollinmemready() != MX25L25645G_OK)
    {
        return MX25L25645G_ERROR_AUTOPOLLING; // 自动轮询失败
    }
    return MX25L25645G_OK; // 读取缓冲区数据成功
}

static void mx25_read(uint32_t address, uint8_t *buff, uint32_t length)
{
    ASSERT(manual_mx25l25645g_read(address, buff, length) == MX25L25645G_OK);
}

static void mx25_write(uint32_t address, uint8_t *buff, uint32_t length)
{
    ASSERT(manual_mx25l25645g_write(address, buff, length) == MX25L25645G_OK);
}

static void mx25_sector_erase(uint32_t address)
{
    ASSERT(manual_mx25l25645g_sector_erase(address) == MX25L25645G_OK);
}

static void mx25_sector_erase64k(uint32_t address)
{
    ASSERT(manual_mx25l25645g_blockerase_64k(address) == MX25L25645G_OK);
}

static void mx25_init(void)
{
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
        .init = &mx25_init, // 只有手动模式才能使用这些接口
        .read = &mx25_read,
        .write = &mx25_write,
        .erase = &mx25_sector_erase,
        .erase_block = &mx25_sector_erase64k,
    },
};

// 手动模式
void flash_manual_mode(void)
{
    uint32_t mx25_id;
    ASSERT(manual_mx25l25645g_reset() == MX25L25645G_OK);
    ASSERT(manual_mx25l25645g_manual_mode_enter() == MX25L25645G_OK);
    mx25_id = manual_mx25l25645g_id_read(); // 读取设备ID
    // log_info("MX25_ID = 0x%08X\r\n", mx25_id); // 打印ID
    ASSERT(mx25_id == MX25L25645G_ID);

    log_info(COLOR_H_YELLOW "qflash mode: [manual]");
}

/**
 * @file w25q32fvss.cc
 * @author WittXie
 * @brief W25Q32FVSS QSPI驱动
 * @version 0.1
 * @date 2024-09-22
 * @note W25Q32FVSS
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
#include <stdint.h>
#include <string.h>

//  W25Q32FVSS 芯片信息
#define W25Q32FVSS_ID 0xEF4016        // W25Q32FVSS ID
#define W25Q32FVSS_PAGE_SIZE 0x100    // 页 256B 最大单次写入单位
#define W25Q32FVSS_SECTOR_SIZE 0x1000 // 扇区 4KB 最小单次擦除单位
#define W25Q32FVSS_BLOCK_SIZE 0x10000 // 块 64KB
#define W25Q32FVSS_CHIP_SIZE 0x400000 // 4MB

// W25Q32FVSS 超时
#define W25Q32FVSS_CHIP_ERASE_TIMEOUT 250000
#define W25Q32FVSS_SECTOR_ERASE_TIMEOUT 800

// W25Q32FVSS 指令集
#define W25Q32FVSS_CMD_WRITE_ENABLE 0x06
#define W25Q32FVSS_CMD_WRITE_DISABLE 0x04
#define W25Q32FVSS_CMD_READ_STATUS_REG1 0X05        // 读状态寄存器1
#define W25Q32FVSS_CMD_ENABLE_RESET 0x66            // 使能复位
#define W25Q32FVSS_CMD_RESET_DEVICE 0x99            // 复位器件
#define W25Q32FVSS_CMD_JEDEC_ID 0x9F                // JEDEC ID
#define W25Q32FVSS_CMD_SECTOR_ERASE 0x20            // 扇区擦除，4K字节， 参考擦除时间 45ms
#define W25Q32FVSS_CMD_BLOCK_ERASE_32K 0x52         // 块擦除，  32K字节，参考擦除时间 120ms
#define W25Q32FVSS_CMD_BLOCK_ERASE_64K 0xD8         // 块擦除，  64K字节，参考擦除时间 150ms
#define W25Q32FVSS_CMD_CHIP_ERASE 0xC7              // 整片擦除，参考擦除时间 20S
#define W25Q32FVSS_CMD_QUAD_INPUT_PAGE_PROGRAM 0x32 // 1-1-4模式下(1线指令1线地址4线数据)，页编程指令，参考写入时间 0.4ms
#define W25Q32FVSS_CMD_FAST_READ_QUAD_IO 0xEB       // 1-4-4模式下(1线指令4线地址4线数据)，快速读取指令
#define W25Q32FVSS_CMD_READ_STATUS_REG1 0X05        // 读状态寄存器1

// 状态寄存器1
#define W25Q32FVSS_STATUS_REG1_BUSY 0x01 // 读状态寄存器1的第0位（只读），Busy标志位，当正在擦除/写入数据/写命令时会被置1
#define W25Q32FVSS_STATUS_REG1_WEL 0x02  // 读状态寄存器1的第1位（只读），WEL写使能标志位，该标志位为1时，代表可以进行写操作

static uint8_t w25q32fvss_auto_polling_mem_ready(uint32_t timeout)
{
    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;     // 1线指令模式
    s_command.AddressMode = QSPI_ADDRESS_NONE;               // 无地址模式
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; //    无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;               // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;           //    每次传输数据都发送指令
    s_command.DataMode = QSPI_DATA_1_LINE;                   // 1线数据模式
    s_command.DummyCycles = 0;                               //    空周期个数
    s_command.Instruction = W25Q32FVSS_CMD_READ_STATUS_REG1; // 读状态信息寄存器

    QSPI_AutoPollingTypeDef s_config;
    memset(&s_config, 0, sizeof(QSPI_AutoPollingTypeDef));
    s_config.Match = 0;                                  //    匹配值
    s_config.MatchMode = QSPI_MATCH_MODE_AND;            //    与运算
    s_config.Interval = 0x10;                            //    轮询间隔
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE; // 自动停止模式
    s_config.StatusBytesSize = 1;                        //    状态字节数
    s_config.Mask = W25Q32FVSS_STATUS_REG1_BUSY;         // 对在轮询模式下接收的状态字节进行屏蔽，只比较需要用到的位

    uint8_t ret = HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, timeout);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi auto polling failed, ret = %d.\r\n", ret);
        return ret;
    }
    return HAL_OK;
}

static uint8_t w25q32fvss_reset(void)
{
    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;     // 1线指令模式
    s_command.AddressMode = QSPI_ADDRESS_NONE;               // 无地址模式
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; // 无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;               // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;           // 每次传输数据都发送指令
    s_command.DataMode = QSPI_DATA_NONE;                     // 无数据模式
    s_command.DummyCycles = 0;                               // 空周期个数
    s_command.Instruction = W25Q32FVSS_CMD_ENABLE_RESET;     // 执行复位使能命令

    uint8_t ret = HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi enable reset command failed, ret = %d.\r\n", ret);
        return ret;
    }

    s_command.Instruction = W25Q32FVSS_CMD_RESET_DEVICE; // 复位器件命令
    ret = HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi reset device command failed, ret = %d.\r\n", ret);
        return ret;
    }

    ret = w25q32fvss_auto_polling_mem_ready(HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi auto polling after reset failed, ret = %d.\r\n", ret);
        return ret;
    }
    return HAL_OK;
}

static uint8_t w25q32fvss_write_enable(void)
{
    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;     // 1线指令模式
    s_command.AddressMode = QSPI_ADDRESS_NONE;               // 无地址模式
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; // 无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;               // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;           // 每次传输数据都发送指令
    s_command.DataMode = QSPI_DATA_NONE;                     // 无数据模式
    s_command.DummyCycles = 0;                               // 空周期个数
    s_command.Instruction = W25Q32FVSS_CMD_WRITE_ENABLE;     // 发送写使能命令

    uint8_t ret = HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi write enable command failed, ret = %d.\r\n", ret);
        return ret;
    }

    QSPI_AutoPollingTypeDef s_config;
    s_config.Match = 0x02;                               // 匹配值
    s_config.Mask = W25Q32FVSS_STATUS_REG1_WEL;          // 读状态寄存器1的第1位（只读），WEL写使能标志位，该标志位为1时，代表可以进行写操作
    s_config.MatchMode = QSPI_MATCH_MODE_AND;            // 与运算
    s_config.StatusBytesSize = 1;                        // 状态字节数
    s_config.Interval = 0x10;                            // 轮询间隔
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE; // 自动停止模式

    s_command.Instruction = W25Q32FVSS_CMD_READ_STATUS_REG1; // 读状态信息寄存器
    s_command.DataMode = QSPI_DATA_1_LINE;                   // 1线数据模式
    s_command.NbData = 1;                                    // 数据长度

    ret = HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi auto polling after write enable failed, ret = %d.\r\n", ret);
        return ret;
    }
    return HAL_OK;
}

static void w25q32fvss_erase_chip(void)
{
    int8_t ret;
    ret = w25q32fvss_write_enable();
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi write enable failed during chip erase, ret = %d.\r\n", ret);
        return;
    }

    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;     // 1线指令模式
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;            // 24位地址模式
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; //    无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;               // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;           // 每次传输数据都发送指令
    s_command.AddressMode = QSPI_ADDRESS_NONE;               // 无地址
    s_command.DataMode = QSPI_DATA_NONE;                     // 无数据
    s_command.DummyCycles = 0;                               // 空周期个数
    s_command.Instruction = W25Q32FVSS_CMD_CHIP_ERASE;       // 擦除命令，进行整片擦除

    ret = HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi chip erase command failed, ret = %d.\r\n", ret);
        return;
    }

    QSPI_AutoPollingTypeDef s_config;                    // 轮询等待配置参数
    s_config.Match = 0;                                  //    匹配值
    s_config.MatchMode = QSPI_MATCH_MODE_AND;            //    与运算
    s_config.Interval = 0x10;                            //    轮询间隔
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE; // 自动停止模式
    s_config.StatusBytesSize = 1;                        //    状态字节数
    s_config.Mask = W25Q32FVSS_STATUS_REG1_BUSY;         // 对在轮询模式下接收的状态字节进行屏蔽，只比较需要用到的位

    s_command.Instruction = W25Q32FVSS_CMD_READ_STATUS_REG1; // 读状态信息寄存器
    s_command.DataMode = QSPI_DATA_1_LINE;                   // 1线数据模式
    s_command.NbData = 1;                                    // 数据长度

    ret = HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, W25Q32FVSS_CHIP_ERASE_TIMEOUT);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi chip erase failed, ret = %d.\r\n", ret);
        return;
    }
}

static uint32_t w25q32fvss_read_id(void)
{
    uint32_t temp = 0;
    uint8_t buff[3];

    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;     // 1线指令模式
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;            // 24位地址
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; // 无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;               // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;           // 每次传输数据都发送指令
    s_command.AddressMode = QSPI_ADDRESS_NONE;               // 无地址模式
    s_command.DataMode = QSPI_DATA_1_LINE;                   // 1线数据模式
    s_command.DummyCycles = 0;                               // 空周期个数
    s_command.NbData = 3;                                    // 传输数据的长度
    s_command.Instruction = W25Q32FVSS_CMD_JEDEC_ID;         // 执行读器件ID命令

    ASSERT(HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) == HAL_OK);
    ASSERT(HAL_QSPI_Receive(&hqspi, buff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) == HAL_OK);
    temp = (buff[2] | buff[1] << 8) | (buff[0] << 16);
    return temp;
}

static uint64_t w25q32fvss_read_unique_id(void)
{
    uint64_t temp = 0;
    uint8_t buff[8];

    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;     // 1线指令模式
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;            // 24位地址
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; // 无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;               // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;           // 每次传输数据都发送指令
    s_command.AddressMode = QSPI_ADDRESS_NONE;               // 无地址模式
    s_command.DataMode = QSPI_DATA_1_LINE;                   // 1线数据模式
    s_command.DummyCycles = 4;                               // 空周期个数
    s_command.NbData = 8;                                    // 传输数据的长度
    s_command.Instruction = 0x4B;                            // 执行读器件ID命令
    ASSERT(HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) == HAL_OK);
    ASSERT(HAL_QSPI_Receive(&hqspi, buff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) == HAL_OK);

    temp = ((uint64_t)buff[7] << 56) | ((uint64_t)buff[6] << 48) | ((uint64_t)buff[5] << 40) | ((uint64_t)buff[4] << 32) |
           ((uint64_t)buff[3] << 24) | ((uint64_t)buff[2] << 16) | ((uint64_t)buff[1] << 8) | ((uint64_t)buff[0]);

    return temp;
}

static void w25q32fvss_sector_erase(uint32_t sector_address)
{
    ASSERT(sector_address % W25Q32FVSS_SECTOR_SIZE == 0);

    int8_t ret;
    ret = w25q32fvss_write_enable();
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi write enable failed during sector erase, ret = %d.\r\n", ret);
        return;
    }

    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.Instruction = W25Q32FVSS_CMD_SECTOR_ERASE;     // 扇区擦除命令
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;     // 1线指令模式
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;            // 24位地址模式
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; // 无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;               // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;           // 每次传输数据都发送指令
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;             // 1线地址模式
    s_command.DataMode = QSPI_DATA_NONE;                     // 无数据
    s_command.NbData = 0;                                    // 数据长度
    s_command.DummyCycles = 0;                               // 空周期个数
    s_command.Address = sector_address;                      // 要擦除的地址

    ret = HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi erase sector command failed, ret = %d.\r\n", ret);
        return;
    }

    ret = w25q32fvss_auto_polling_mem_ready(HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi auto polling after sector erase failed, ret = %d.\r\n", ret);
        return;
    }
}

static void w25q32fvss_read(uint32_t address, uint8_t *buff, uint32_t length)
{
    ASSERT(buff != NULL);
    ASSERT(length != 0);

    int8_t ret;

    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;      // 1线指令模式
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;             // 24位地址
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  // 无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;                // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;   // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;            // 每次传输数据都发送指令
    s_command.AddressMode = QSPI_ADDRESS_4_LINES;             // 4线地址模式
    s_command.DataMode = QSPI_DATA_4_LINES;                   // 4线数据模式
    s_command.DummyCycles = 6;                                // 空周期个数
    s_command.NbData = length;                                // 数据长度，最大不能超过flash芯片的大小
    s_command.Address = address;                              // 要读取 W25Qxx 的地址
    s_command.Instruction = W25Q32FVSS_CMD_FAST_READ_QUAD_IO; // 1-4-4模式下(1线指令4线地址4线数据)，快速读取指令

    ret = HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi read command failed, ret = %d.\r\n", ret);
        return;
    }

    ret = HAL_QSPI_Receive(&hqspi, buff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi receive data failed, ret = %d.\r\n", ret);
        return;
    }

    ret = w25q32fvss_auto_polling_mem_ready(HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi auto polling after read failed, ret = %d.\r\n", ret);
        return;
    }
}

static void w25q32fvss_write(uint32_t address, uint8_t *buff, uint32_t length)
{
    int8_t ret;

    if (length > W25Q32FVSS_PAGE_SIZE)
    {
        ERROR("length(%d) > W25Q32FVSS_PAGE_SIZE(%d).\r\n", length, W25Q32FVSS_PAGE_SIZE);
    }

    ASSERT(buff != NULL);
    ASSERT(length != 0);
    ASSERT(length <= W25Q32FVSS_PAGE_SIZE);

    ret = w25q32fvss_write_enable();
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi write enable failed during write operation, ret = %d.\r\n", ret);
        return;
    }

    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;            // 1线指令模式
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;                   // 24位地址
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;        // 无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;                      // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;         // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;                  // 每次传输数据都发送指令
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;                    // 1线地址模式
    s_command.DataMode = QSPI_DATA_4_LINES;                         // 4线数据模式
    s_command.DummyCycles = 0;                                      // 空周期个数
    s_command.NbData = length;                                      // 数据长度，最大只能256字节
    s_command.Address = address;                                    // 要写入 W25Qxx 的地址
    s_command.Instruction = W25Q32FVSS_CMD_QUAD_INPUT_PAGE_PROGRAM; // 1-1-4模式下(1线指令1线地址4线数据)，页编程指令

    ret = HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi write command failed, ret = %d.\r\n", ret);
        return;
    }

    ret = HAL_QSPI_Transmit(&hqspi, buff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi transmit data failed, ret = %d.\r\n", ret);
        return;
    }

    ret = w25q32fvss_auto_polling_mem_ready(HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("w25qxx qspi auto polling after write failed, ret = %d.\r\n", ret);
        return;
    }
}

static uint8_t w25q32fvss_memory_mapped_mode(void)
{
    if (w25q32fvss_reset() != HAL_OK)
    {
        ERROR("w25qxx qspi reset failed.");
        return HAL_ERROR;
    }

    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;      // 1线指令模式
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;             // 24位地址
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  // 无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;                // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;   // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;            // 每次传输数据都发送指令
    s_command.AddressMode = QSPI_ADDRESS_4_LINES;             // 4线地址模式
    s_command.DataMode = QSPI_DATA_4_LINES;                   // 4线数据模式
    s_command.DummyCycles = 6;                                // 空周期个数
    s_command.Instruction = W25Q32FVSS_CMD_FAST_READ_QUAD_IO; // 1-4-4模式下(1线指令4线地址4线数据)，快速读取指令

    QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;
    memset(&s_mem_mapped_cfg, 0, sizeof(QSPI_MemoryMappedTypeDef));
    s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE; // 禁用超时计数器, nCS 保持激活状态
    s_mem_mapped_cfg.TimeOutPeriod = 0;                                // 超时判断周期
    if (HAL_QSPI_MemoryMapped(&hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK)
    {
        ERROR("w25qxx qspi memory mapped mode failed.");
        return HAL_ERROR;
    }

    return HAL_OK;
}

static void w25q32fvss_manual_init(void)
{
    ASSERT(w25q32fvss_reset() == HAL_OK);

    QSPI_CommandTypeDef s_command;
    memset(&s_command, 0, sizeof(QSPI_CommandTypeDef));
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;     // 1线指令模式
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;            // 24位地址
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; // 无交替字节
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;               // 禁止DDR模式
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;           // 每次传输数据都发送指令
    s_command.AddressMode = QSPI_ADDRESS_NONE;               // 无地址模式
    s_command.DataMode = QSPI_DATA_1_LINE;                   // 1线数据模式
    s_command.DummyCycles = 0;                               // 空周期个数
    s_command.NbData = 1;                                    // 传输数据的长度
    s_command.Instruction = 0x01;                            // Write Status Register-1

    uint8_t value = 0x06;
    ASSERT(HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) == HAL_OK);
    ASSERT(HAL_QSPI_Transmit(&hqspi, &value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) == HAL_OK);
    ASSERT(w25q32fvss_auto_polling_mem_ready(W25Q32FVSS_SECTOR_ERASE_TIMEOUT) == HAL_OK);
}

static void w25q32fvss_maped_init(void)
{
    ASSERT(w25q32fvss_reset() == HAL_OK);
    ASSERT(w25q32fvss_memory_mapped_mode() == HAL_OK);
}

flash_t g_flash_w25q32 = {
    .cfg = {
        .name = "w25q32fvss",
        .write_max_size = W25Q32FVSS_PAGE_SIZE,
        .erase_min_size = W25Q32FVSS_SECTOR_SIZE,
        .chip_size = W25Q32FVSS_CHIP_SIZE,
    },
    .ops = {
        .init = &w25q32fvss_manual_init, // 只有手动模式才能使用这些接口
        .read = &w25q32fvss_read,
        .write = &w25q32fvss_write,
        .erase = &w25q32fvss_sector_erase,
    },
};

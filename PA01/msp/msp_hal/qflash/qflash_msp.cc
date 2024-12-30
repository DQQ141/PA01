/**
 * @file mx25l25645g.cc
 * @author Sun
 * @brief mx25l25645g QSPI驱动
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
#include "./../msp_env.h"
#include "./qspi.cc"
#include <stdint.h>
#include <string.h>

#undef ASSERT
#define ASSERT(_x)    \
    {                 \
        if (!(_x))    \
            while (1) \
            {         \
            }         \
    }
#undef ERROR
#define ERROR(format, ...)

#define MX25L25645G_CMD_READ_STATUS_REG1 0X05
#define MX25L25645G_CMD_ENABLE_RESET 0x66
#define MX25L25645G_CMD_RESET_DEVICE 0x99
#define MX25L25645G_CMD_Enter4B 0XB7
#define MX25L25645G_CMD_ReadConfig_REG1 0x15
#define MX25L25645G_CMD_ReadStatus_REG1 0x05
#define MX25L25645G_CMD_WriteEnable 0X06
#define MX25L25645G_STATUS_REG1_BUSY 0x01
#define MX25L25645G_STATUS_REG1_WEL 0x02

static uint8_t mx25l25645g_auto_polling_mode(QSPI_HandleTypeDef *hqspi, uint32_t timeout)
{
    QSPI_CommandTypeDef s_command = {0};
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.Instruction = MX25L25645G_CMD_READ_STATUS_REG1;

    QSPI_AutoPollingTypeDef s_config = {0};
    s_config.Match = 0;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    s_config.StatusBytesSize = 1;
    s_config.Mask = MX25L25645G_STATUS_REG1_BUSY;

    uint8_t ret = HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, timeout);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi auto polling failed, ret = %d.\r\n", ret);
        return ret;
    }
    return HAL_OK;
}

static uint8_t mx25l25645g_status_read(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;
    uint8_t QSPI_ReceiveBuff[1];
    uint8_t StatusR;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 1;
    s_command.Instruction = MX25L25645G_CMD_ReadStatus_REG1;

    uint8_t ret = HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        return ret;
    }

    ret = HAL_QSPI_Receive(hqspi, QSPI_ReceiveBuff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        return ret;
    }
    StatusR = QSPI_ReceiveBuff[0];

    return StatusR;
}

static uint8_t mx25l25645g_configuration_read(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;
    uint8_t QSPI_ReceiveBuff[1];
    uint8_t StatusR;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 1;
    s_command.Instruction = MX25L25645G_CMD_ReadConfig_REG1;

    uint8_t ret = HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        return ret;
    }

    ret = HAL_QSPI_Receive(hqspi, QSPI_ReceiveBuff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        return ret;
    }
    StatusR = QSPI_ReceiveBuff[0];

    return StatusR;
}

static int8_t mx25l25645g_enter4byte_mode(QSPI_HandleTypeDef *hqspi)
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
    s_command.Instruction = MX25L25645G_CMD_Enter4B;

    uint8_t ret = HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        return ret;
    }

    ret = mx25l25645g_auto_polling_mode(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        return ret;
    }

    return HAL_OK;
}

static int8_t mx25l25645g_write_enable(QSPI_HandleTypeDef *hqspi)
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
    s_command.Instruction = MX25L25645G_CMD_WriteEnable;

    uint8_t ret = HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi write enable command failed 1, ret = %d.\r\n", ret);
        return ret;
    }

    s_config.Match = 0x02;
    s_config.Mask = MX25L25645G_STATUS_REG1_WEL;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = MX25L25645G_CMD_ReadStatus_REG1;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 1;
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;

    ret = HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi write enable command failed 3, ret = %d.\r\n", ret);
        return ret;
    }

    return HAL_OK;
}

static int8_t mx25l25645g_write_status_configuration(QSPI_HandleTypeDef *hqspi)
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
    s_command.Instruction = 0x01;
    s_command.NbData = 2;

    uint8_t pBuffer[2];

    pBuffer[0] = 0x40;
    pBuffer[1] = 0x20;

    uint8_t ret = mx25l25645g_write_enable(hqspi);

    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi write enable command failed 4, ret = %d.\r\n", ret);
        return ret;
    }

    ret = HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi write status configuration resgister failed, ret = %d.\r\n", ret);
        return ret;
    }

    ret = HAL_QSPI_Transmit(hqspi, pBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi write status configuration resgister failed 1, ret = %d.\r\n", ret);
        return ret;
    }

    ret = mx25l25645g_auto_polling_mode(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi auto polling after reset failed , ret = %d.\r\n", ret);
        return ret;
    }

    return HAL_OK;
}

static uint8_t mx25l25645g_reset(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command = {0};
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.Instruction = MX25L25645G_CMD_ENABLE_RESET;

    uint8_t ret = HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi enable reset command failed, ret = %d.\r\n", ret);
        return ret;
    }

    ret = mx25l25645g_auto_polling_mode(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi auto polling after reset failed, ret = %d.\r\n", ret);
        ret = HAL_OK;
        return ret;
    }
    s_command.Instruction = MX25L25645G_CMD_RESET_DEVICE;
    ret = HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi reset device command failed, ret = %d.\r\n", ret);
        return ret;
    }

    ret = mx25l25645g_auto_polling_mode(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi auto polling after reset failed, ret = %d.\r\n", ret);
        return ret;
    }
    return HAL_OK;
}

static uint8_t mx25l25645g_memory_mapped_mode(QSPI_HandleTypeDef *hqspi)
{
    uint32_t ticks;
    uint8_t statusr;
    if (mx25l25645g_reset(hqspi) != HAL_OK)
    {
        ERROR("MX25L25645G qspi reset failed.");
        return HAL_ERROR;
    }

    ticks = HAL_GetTick();
    do
    {
    } while (HAL_GetTick() - ticks < 100);

    uint8_t ret = mx25l25645g_write_status_configuration(hqspi);
    if (ret != HAL_OK)
    {
        ERROR("MX25L25645G qspi memory mapped failed, ret = %d.\r\n", ret);
        return HAL_ERROR;
    }
    mx25l25645g_enter4byte_mode(hqspi);
    ret = mx25l25645g_status_read(hqspi);
    ret = mx25l25645g_configuration_read(hqspi);

    QSPI_CommandTypeDef s_command = {0};
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_ENABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_4_LINES;
    s_command.DataMode = QSPI_DATA_4_LINES;
    s_command.DummyCycles = 6;
    s_command.Instruction = 0xEE;

    QSPI_MemoryMappedTypeDef s_mem_mapped_cfg = {0};
    s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    s_mem_mapped_cfg.TimeOutPeriod = 0;
    if (QSPI_MemoryMapped(hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK)
    {
        ERROR("MX25L25645G qspi memory mapped mode failed.");
        return HAL_ERROR;
    }

    return HAL_OK;
}

// 映射模式
void flash_remap_mode(void)
{
    // 映射模式
    ASSERT(mx25l25645g_reset(&hqspi) == HAL_OK);
    ASSERT(mx25l25645g_memory_mapped_mode(&hqspi) == HAL_OK);
}

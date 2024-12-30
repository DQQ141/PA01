/**
 * @file sdram_msp.cc
 * @author WittXie
 * @brief SDRAM初始化
 * @version 0.1
 * @date 2024-09-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../msp_env.h"

#define SDRAM_MODEREG_BURST_LENGTH_1 ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2 ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4 ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8 ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2 ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((uint16_t)0x0200)

static void sdram_send_smd(SDRAM_HandleTypeDef *hsdram, uint8_t cmd, uint8_t refresh, uint32_t regval)
{
    FMC_SDRAM_CommandTypeDef Command = {0};
    Command.CommandMode = cmd;                          // 命令
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1; // 目标SDRAM存储区域
    Command.AutoRefreshNumber = refresh;                // 自刷新次数
    Command.ModeRegisterDefinition = regval;            // 要写入模式寄存器的值
    FMC_SDRAM_SendCommand(hsdram->Instance, &Command, 0XFFFF);
}

void sdram_msp_init(void)
{
    SDRAM_HandleTypeDef *hsdram = &hsdram1;
    __IO uint32_t tmpmrd = 0;

    /* 1- 时钟使能命令 */
    sdram_send_smd(hsdram, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);
    /* 2- 插入延迟，至少100us */
    HAL_Delay(100);
    /* 3- 整个SDRAM预充电命令，PALL(precharge all) */
    sdram_send_smd(hsdram, FMC_SDRAM_CMD_PALL, 1, 0);

    /* 4- 自动刷新命令 */
    sdram_send_smd(hsdram, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);

    /* 5- 配置SDRAM模式寄存器 */
    tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1 |
             SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
             SDRAM_MODEREG_CAS_LATENCY_3 |
             SDRAM_MODEREG_OPERATING_MODE_STANDARD |
             SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
    sdram_send_smd(hsdram, FMC_SDRAM_CMD_LOAD_MODE, 1, tmpmrd);

    // SDRAM refresh = period / rows * SDRAM时钟速度 – 20
    //               = 64000us / 4096 * (FMC:240MHz/2分频) - 20
    //               = 1855
    FMC_SDRAM_ProgramRefreshRate(hsdram->Instance, 1855);
}

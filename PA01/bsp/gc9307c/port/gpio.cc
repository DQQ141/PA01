/**
 * @file gpio.cc
 * @author WittXie
 * @brief  GPIO模拟SPI接口
 * @version 0.1
 * @date 2024-10-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../bsp.h"
#include "./../gc9307c_bsp.h"

// 接线如下：
// PB5 LCM_SPLMOSI
// PG10 LCM_SPI_CSN
// PG9 LCM_SPI_MISO
// PG11 LCM_SPI_CLK
// PD7 LCM_RSTN
// PG12 LCM_RS
// PB7 LCM_FMARK

// IO口模拟SPI传输

// 定义引脚
// #define LCM_SPI_MISO_PIN GPIO_PIN_5 // 临时
// #define LCM_SPI_MISO_PORT GPIOB     // 临时
// #define LCM_SPLMOSI_PIN GPIO_PIN_9  // 临时
// #define LCM_SPLMOSI_PORT GPIOG      // 临时

#define LCM_SPI_MISO_PIN GPIO_PIN_9
#define LCM_SPI_MISO_PORT GPIOG

#define LCM_SPLMOSI_PIN GPIO_PIN_5
#define LCM_SPLMOSI_PORT GPIOB

#define LCM_SPI_CSN_PIN GPIO_PIN_10
#define LCM_SPI_CSN_PORT GPIOG

#define LCM_SPI_CLK_PIN GPIO_PIN_11
#define LCM_SPI_CLK_PORT GPIOG

#define LCM_RSTN_PIN GPIO_PIN_7
#define LCM_RSTN_PORT GPIOD

#define LCM_RS_PIN GPIO_PIN_12
#define LCM_RS_PORT GPIOG

#define LCM_FMARK_PIN GPIO_PIN_7
#define LCM_FMARK_PORT GPIOB

#define LCD_RS_TO_DATA HAL_GPIO_WritePin(LCM_RS_PORT, LCM_RS_PIN, GPIO_PIN_SET);
#define LCD_RS_TO_CMD HAL_GPIO_WritePin(LCM_RS_PORT, LCM_RS_PIN, GPIO_PIN_RESET);
#define LCD_CSN_TO_DISABLE HAL_GPIO_WritePin(LCM_SPI_CSN_PORT, LCM_SPI_CSN_PIN, GPIO_PIN_SET);
#define LCD_CSN_TO_ENABLE HAL_GPIO_WritePin(LCM_SPI_CSN_PORT, LCM_SPI_CSN_PIN, GPIO_PIN_RESET);
#define LCD_RSTN_TO_RST HAL_GPIO_WritePin(LCM_RSTN_PORT, LCM_RSTN_PIN, GPIO_PIN_RESET);
#define LCD_RSTN_TO_NORMAL HAL_GPIO_WritePin(LCM_RSTN_PORT, LCM_RSTN_PIN, GPIO_PIN_SET);
#define LCD_CLK_TO_1 HAL_GPIO_WritePin(LCM_SPI_CLK_PORT, LCM_SPI_CLK_PIN, GPIO_PIN_SET);
#define LCD_CLK_TO_0 HAL_GPIO_WritePin(LCM_SPI_CLK_PORT, LCM_SPI_CLK_PIN, GPIO_PIN_RESET);
#define LCD_SPLMOSI_TO_1 HAL_GPIO_WritePin(LCM_SPLMOSI_PORT, LCM_SPLMOSI_PIN, GPIO_PIN_SET);
#define LCD_SPLMOSI_TO_0 HAL_GPIO_WritePin(LCM_SPLMOSI_PORT, LCM_SPLMOSI_PIN, GPIO_PIN_RESET);

void lcd_gpio_init(void)
{
    HAL_SPI_DeInit(&hspi1);
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LCM_SPLMOSI (PB5)
    GPIO_InitStruct.Pin = LCM_SPLMOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LCM_SPLMOSI_PORT, &GPIO_InitStruct);

    // LCM_SPI_CSN (PG10)
    GPIO_InitStruct.Pin = LCM_SPI_CSN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LCM_SPI_CSN_PORT, &GPIO_InitStruct);

    // LCM_SPI_MISO (PG9)
    GPIO_InitStruct.Pin = LCM_SPI_MISO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LCM_SPI_MISO_PORT, &GPIO_InitStruct);

    // LCM_SPI_CLK (PG11)
    GPIO_InitStruct.Pin = LCM_SPI_CLK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LCM_SPI_CLK_PORT, &GPIO_InitStruct);

    // LCM_RSTN (PD7)
    GPIO_InitStruct.Pin = LCM_RSTN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LCM_RSTN_PORT, &GPIO_InitStruct);

    // LCM_RS (PG12)
    GPIO_InitStruct.Pin = LCM_RS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LCM_RS_PORT, &GPIO_InitStruct);

    // LCM_FMARK (PB7)
    GPIO_InitStruct.Pin = LCM_FMARK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LCM_FMARK_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET); // 使能SPI
}

// 写控制命令
static void lcd_gpio_write_cmd(uint8_t cmd)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET); // RS引脚设置为低，表示命令模式

    for (int8_t i = 7; i >= 0; i--)
    {
        LCD_CLK_TO_0; // 时钟低电平
        if (cmd & (1 << i))
        {
            LCD_SPLMOSI_TO_1; // 数据高电平
        }
        else
        {
            LCD_SPLMOSI_TO_0; // 数据低电平
        }

        LCD_CLK_TO_1; // 时钟高电平
    }
}

static void lcd_gpio_write_data(uint8_t *buff, uint64_t length)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET); // RS引脚设置为高，表示数据模式

    for (uint32_t i = 0; i < length; i++)
    {
        for (int8_t j = 7; j >= 0; j--)
        {
            LCD_CLK_TO_0; // 时钟低电平
            if (buff[i] & (1 << j))
            {
                LCD_SPLMOSI_TO_1; // 数据高电平
            }
            else
            {
                LCD_SPLMOSI_TO_0; // 数据低电平
            }
            LCD_CLK_TO_1; // 时钟高电平
        }
    }
}

static void lcd_gpio_reset(void)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET); // 拉高复位引脚
    os_sleep(50);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET); // 拉低复位引脚
    os_sleep(50);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET); // 拉高复位引脚
    os_sleep(50);
}
static uint16_t lcd_buff[320 * 240] __section_sdram;
gc9307c_t g_gc9307c_lcd = {
    .cfg = {
        .name = "g_gc9307c_lcd",
        .buff = lcd_buff,
        .width = 320,
        .height = 240,
        .direction = GC9307C_DIR_HORIZONTAL_FLIP_Y,
        .is_auto_sync_mode = false, // 关闭自动同步模式
    },
    .ops = {
        .init = lcd_gpio_init,
        .reset = lcd_gpio_reset,
        .write_command = lcd_gpio_write_cmd,
        .write_data = lcd_gpio_write_data,
    },
};

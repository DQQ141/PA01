/**
 * @file spi_dma_map.cc
 * @author WittXie
 * @brief 屏幕spi驱动
 * @versspin 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../gc9307c_bsp.h"

// 接线如下：SPI1
// PG9 LCM_SPI_MISO
// PB5 LCM_SPLMOSI
// PG11 LCM_SPI_CLK
// PB7 LCM_FMARK

// GPIOD GPIO_PIN_7 LCM_RSTN
// GPIOG GPIO_PIN_10 LCM_SPI_CSN
// GPIOG GPIO_PIN_12 LCM_RS

#define LCM_SPI_CS_ENABLE() HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET)
#define LCM_SPI_CS_DISABLE() HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET)

#define LCM_SPI_RS_DATA() HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET)
#define LCM_SPI_RS_CMD() HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET)

#define LCD_BUFFER_SIZE 0xFFFF
static uint16_t lcd_buff[LCD_BUFFER_SIZE] __section_sdram;
void lcd_delay(void)
{
    os_sleep(120);
}
void lcd_spi_auto_sync_start(uint8_t *buff, uint32_t length)
{
    LCM_SPI_RS_DATA(); // 数据模式
    HAL_SPI_Transmit_DMA(&hspi1, buff, length);
}
static void lcd_spi_init(void)
{
    LCM_SPI_CS_DISABLE(); // 关闭片选
    LCM_SPI_RS_DATA();    // 数据模式
}
static void lcd_spi_reset(void)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET); // 拉高复位引脚
    os_sleep(50);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET); // 拉低复位引脚
    os_sleep(50);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET); // 拉高复位引脚
    os_sleep(120);
}
static void lcd_spi_write_cmd(uint8_t command)
{
    LCM_SPI_CS_ENABLE(); // 使能片选

    // 选择命令模式
    LCM_SPI_RS_CMD(); // RS引脚设置为低，表示命令模式
    HAL_SPI_Transmit(&hspi1, (uint8_t *)&command, 1, HAL_MAX_DELAY);

    os_sleep_until(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_READY, 1000);
    LCM_SPI_CS_DISABLE(); // 关闭片选
    LCM_SPI_RS_DATA();    // 数据模式
}
static void lcd_spi_write_data(uint8_t *buff, uint32_t length)
{
    LCM_SPI_CS_ENABLE(); // 使能片选

    // 选择数据模式
    LCM_SPI_RS_DATA(); // 数据模式

    if (length <= LCD_BUFFER_SIZE)
    {
        HAL_SPI_Transmit(&hspi1, buff, length, HAL_MAX_DELAY);
    }
    else
    {
        // 如果长度大于 0xFFFF，则分块传输
        uint32_t remaining_length = length;
        uint8_t *current_buff = buff;

        while (remaining_length > 0)
        {
            uint16_t chunk_size = (remaining_length > LCD_BUFFER_SIZE) ? LCD_BUFFER_SIZE : (uint16_t)remaining_length;

            // 等待传输完成
            os_sleep_until(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_READY, 1000);
            HAL_SPI_Transmit(&hspi1, current_buff, chunk_size, HAL_MAX_DELAY);

            // 更新剩余长度和缓冲区指针
            remaining_length -= chunk_size;
            current_buff += chunk_size;
        }
    }

    os_sleep_until(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_READY, 1000);
    LCM_SPI_CS_DISABLE(); // 关闭片选
}
gc9307c_t g_gc9307c_lcd = {
    .cfg = {
        .name = "g_gc9307c_lcd",
        .buff = lcd_buff,
        .width = 320,
        .height = 240,
        .direction = GC9307C_DIR_HORIZONTAL_FLIP_Y,
    },
    .ops = {
        .init = lcd_spi_init,
        .reset = lcd_spi_reset,
        .write_command = lcd_spi_write_cmd,
        .write_data = lcd_spi_write_data,
        .delay = lcd_delay,
    },
};

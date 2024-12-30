/**
 * @file i2c.cc
 * @author WittXie
 * @brief IO拓展芯片 AW9523B 端口
 * @versi2cn 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../aw9523b_bsp.h"

#define AW9523B_I2C_ADDR_READ (0b10110111)
#define AW9523B_I2C_ADDR_WRITE (0b10110110)
static void aw9523b_i2c_init(void)
{
}
static void aw9523b_i2c_read(uint8_t addr, uint8_t *buff, uint32_t length)
{
    HAL_I2C_Mem_Read(&hi2c1, AW9523B_I2C_ADDR_READ, addr, I2C_MEMADD_SIZE_8BIT, buff, length, 10);
}
static void aw9523b_i2c_write(uint8_t addr, uint8_t *buff, uint32_t length)
{
    HAL_I2C_Mem_Write(&hi2c1, AW9523B_I2C_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, buff, length, 10);
}
static void aw9523b_i2c_delay(void)
{
    os_sleep(100);
}

aw9523b_t g_aw9523b_exio = {
    .cfg = {
        .name = "g_aw9523b_exio",
        .try_cnt = 20,
    },
    .ops = {
        .init = aw9523b_i2c_init,
        .read = aw9523b_i2c_read,
        .write = aw9523b_i2c_write,
        .delay = aw9523b_i2c_delay,
    },
};

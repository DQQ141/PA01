/**
 * @file i2c.cc
 * @author WittXie
 * @brief IMU拓展芯片 LSM6DSDTR 端口
 * @versi2cn 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../lsm6dsdtr_bsp.h"

#define LSM6DSDTR_I2C_ADDR_READ (0b11010101)
#define LSM6DSDTR_I2C_ADDR_WRITE (0b11010100)
static void lsm6dsdtr_i2c_init(void)
{
}
static void lsm6dsdtr_i2c_delay(void)
{
    os_sleep(30);
}
static void lsm6dsdtr_i2c_read(uint8_t addr, uint8_t *buff, uint32_t length)
{
    HAL_I2C_Mem_Read(&hi2c2, LSM6DSDTR_I2C_ADDR_READ, addr, I2C_MEMADD_SIZE_8BIT, buff, length, 10);
}
static void lsm6dsdtr_i2c_write(uint8_t addr, uint8_t *buff, uint32_t length)
{
    HAL_I2C_Mem_Write(&hi2c2, LSM6DSDTR_I2C_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, buff, length, 10);
}
lsm6dsdtr_t g_lsm6dsdtr_imu = {
    .cfg = {
        .name = "g_lsm6dsdtr_imu",
        .filter_alpha = 0.65, // 滤波系数
        .bias = 9,            // 零漂，可通过静置校准获得
        .config = LSM6DSDTR_DEFAULT_CONFIG,
        .try_cnt = 20,
    },
    .ops = {
        .init = lsm6dsdtr_i2c_init,
        .read = lsm6dsdtr_i2c_read,
        .write = lsm6dsdtr_i2c_write,
        .delay = lsm6dsdtr_i2c_delay,
    },
};

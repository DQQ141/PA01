#include "./lsm6dsdtr.h"
#include <string.h>

// LSM6DSDTR 寄存器定义
#define REG_WHO_AM_I 0x0F   // WHO_AM_I寄存器，用于读取芯片ID
#define REG_CTRL1_XL 0x10   // 控制寄存器1，用于配置加速度计的输出数据速率（ODR）和全量程范围
#define REG_CTRL2_G 0x11    // 控制寄存器2，用于配置陀螺仪的输出数据速率（ODR）和全量程范围
#define REG_CTRL3_C 0x12    // 控制寄存器3，用于配置常规控制设置（如BDU，自动增益控制等）
#define REG_CTRL4_C 0x13    // 控制寄存器4（使能加速度计和陀螺仪）
#define REG_CTRL5_C 0x14    // 控制寄存器5（自检和数据舍入）
#define REG_CTRL6_C 0x15    // 控制寄存器6（加速度计和陀螺仪高性能模式）
#define REG_CTRL7_G 0x16    // 控制寄存器7（陀螺仪滤波器配置）
#define REG_CTRL8_XL 0x17   // 控制寄存器8（加速度计滤波器配置）
#define REG_CTRL9_XL 0x18   // 控制寄存器9（低通滤波器配置）
#define REG_CTRL10_C 0x19   // 控制寄存器10（加速度计自动校准和陀螺仪自动校准配置）
#define REG_OUT_TEMP_L 0x20 // 温度数据低位寄存器，存储温度传感器的低字节
#define REG_OUT_TEMP_H 0x21 // 温度数据高位寄存器，存储温度传感器的高字节
#define REG_OUTX_L_G 0x22   // 陀螺仪X轴数据低位寄存器
#define REG_OUTX_H_G 0x23   // 陀螺仪X轴数据高位寄存器
#define REG_OUTY_L_G 0x24   // 陀螺仪Y轴数据低位寄存器
#define REG_OUTY_H_G 0x25   // 陀螺仪Y轴数据高位寄存器
#define REG_OUTZ_L_G 0x26   // 陀螺仪Z轴数据低位寄存器
#define REG_OUTZ_H_G 0x27   // 陀螺仪Z轴数据高位寄存器
#define REG_OUTX_L_XL 0x28  // 加速度计X轴数据低位寄存器
#define REG_OUTX_H_XL 0x29  // 加速度计X轴数据高位寄存器
#define REG_OUTY_L_XL 0x2A  // 加速度计Y轴数据低位寄存器
#define REG_OUTY_H_XL 0x2B  // 加速度计Y轴数据高位寄存器
#define REG_OUTZ_L_XL 0x2C  // 加速度计Z轴数据低位寄存器
#define REG_OUTZ_H_XL 0x2D  // 加速度计Z轴数据高位寄存器

#define LSM6DSDTR_CHIP_ID (0x6A) // 预期的 LSM6DSDTR 芯片ID

// 初始化LSM6DSDTR
void lsm6dsdtr_init(lsm6dsdtr_t *lsm6dsdtr)
{
    ASSERT(lsm6dsdtr != NULL);
    ASSERT(lsm6dsdtr->cfg.name != NULL);
    ASSERT(lsm6dsdtr->cfg.config.ctrl1_xl != 0); // 这项配置=0 时，无数据输出
    ASSERT(lsm6dsdtr->cfg.config.ctrl2_g != 0);  // 这项配置=0 时，无数据输出
    ASSERT(lsm6dsdtr->cfg.try_cnt != 0);
    ASSERT(lsm6dsdtr->ops.init != NULL);
    ASSERT(lsm6dsdtr->ops.read != NULL);
    ASSERT(lsm6dsdtr->ops.write != NULL);
    ASSERT(lsm6dsdtr->ops.delay != NULL);

    // 初始化硬件接口
    lsm6dsdtr->ops.init();

    lsm6dsdtr->init_try_cnt = 0;
    lsm6dsdtr->flag.value = 0;
}

void lsm6dsdtr_poll(lsm6dsdtr_t *lsm6dsdtr)
{
    // 初始化检查
    if (!lsm6dsdtr->flag.is_inited)
    {
        if (lsm6dsdtr->init_try_cnt < lsm6dsdtr->cfg.try_cnt)
        {
            // 重启设备
            uint8_t ctrl3_c = 0xFF;
            lsm6dsdtr->ops.write(REG_CTRL3_C, &ctrl3_c, 1);
            lsm6dsdtr->ops.delay();

            // 读取ID并打印
            uint8_t id = 0;
            lsm6dsdtr->ops.read(REG_WHO_AM_I, &id, 1);
            if (id == LSM6DSDTR_CHIP_ID)
            {
                INFO("lsm6dsdtr chip ID: 0x%02X.", id);
                // 将参数写入芯片
                lsm6dsdtr->ops.write(REG_CTRL1_XL, &lsm6dsdtr->cfg.config.ctrl1_xl, 1);
                lsm6dsdtr->ops.write(REG_CTRL2_G, &lsm6dsdtr->cfg.config.ctrl2_g, 1);
                lsm6dsdtr->ops.write(REG_CTRL3_C, &lsm6dsdtr->cfg.config.ctrl3_c, 1);
                lsm6dsdtr->ops.write(REG_CTRL4_C, &lsm6dsdtr->cfg.config.ctrl4_c, 1);
                lsm6dsdtr->ops.write(REG_CTRL5_C, &lsm6dsdtr->cfg.config.ctrl5_c, 1);
                lsm6dsdtr->ops.write(REG_CTRL6_C, &lsm6dsdtr->cfg.config.ctrl6_c, 1);
                lsm6dsdtr->ops.write(REG_CTRL7_G, &lsm6dsdtr->cfg.config.ctrl7_g, 1);
                lsm6dsdtr->ops.write(REG_CTRL8_XL, &lsm6dsdtr->cfg.config.ctrl8_xl, 1);
                lsm6dsdtr->ops.write(REG_CTRL9_XL, &lsm6dsdtr->cfg.config.ctrl9_xl, 1);
                lsm6dsdtr->ops.write(REG_CTRL10_C, &lsm6dsdtr->cfg.config.ctrl10_c, 1);

                // 通过配置计算灵敏度
                lsm6dsdtr->data.gyro_sensitivity = powf(2, lsm6dsdtr->cfg.config.ctrl2_g_bits.fs_g) * 250.0f / 16383.0f;

                lsm6dsdtr->flag.is_inited = true;
            }
            lsm6dsdtr->init_try_cnt++;
        }
        else if (lsm6dsdtr->init_try_cnt == lsm6dsdtr->cfg.try_cnt)
        {
            ERROR("[%s] init failed.", lsm6dsdtr->cfg.name);
            lsm6dsdtr->init_try_cnt++;
        }
        return; // 如果未初始化完成，直接返回
    }

    lsm6dsdtr->flag.is_running = true;

    uint8_t temp_l, temp_h;
    uint8_t accel_x_l, accel_x_h, accel_y_l, accel_y_h, accel_z_l, accel_z_h;
    uint8_t gyro_x_l, gyro_x_h, gyro_y_l, gyro_y_h, gyro_z_l, gyro_z_h;

    if (lsm6dsdtr->data.timestamp == 0)
    {
        lsm6dsdtr->data.timestamp = TIMESTAMP_US_GET(); // 获取当前时间戳
    }

    // 读取数据
    lsm6dsdtr->ops.read(REG_OUT_TEMP_L, &temp_l, 1); // 读取温度数据
    lsm6dsdtr->ops.read(REG_OUT_TEMP_H, &temp_h, 1);
    lsm6dsdtr->ops.read(REG_OUTX_L_G, &gyro_x_l, 1); // 读取陀螺仪数据
    lsm6dsdtr->ops.read(REG_OUTX_H_G, &gyro_x_h, 1);
    lsm6dsdtr->ops.read(REG_OUTY_L_G, &gyro_y_l, 1);
    lsm6dsdtr->ops.read(REG_OUTY_H_G, &gyro_y_h, 1);
    lsm6dsdtr->ops.read(REG_OUTZ_L_G, &gyro_z_l, 1);
    lsm6dsdtr->ops.read(REG_OUTZ_H_G, &gyro_z_h, 1);
    lsm6dsdtr->ops.read(REG_OUTX_L_XL, &accel_x_l, 1); // 读取加速度计数据
    lsm6dsdtr->ops.read(REG_OUTX_H_XL, &accel_x_h, 1);
    lsm6dsdtr->ops.read(REG_OUTY_L_XL, &accel_y_l, 1);
    lsm6dsdtr->ops.read(REG_OUTY_H_XL, &accel_y_h, 1);
    lsm6dsdtr->ops.read(REG_OUTZ_L_XL, &accel_z_l, 1);
    lsm6dsdtr->ops.read(REG_OUTZ_H_XL, &accel_z_h, 1);
    uint64_t current_timestamp = TIMESTAMP_US_GET(); // 获取当前时间戳

    // 计算数据
    lsm6dsdtr->data.temperature = (int16_t)((temp_h << 8) | temp_l); // 计算温度
    lsm6dsdtr->data.gyro.x = (int16_t)((gyro_x_h << 8) | gyro_x_l);  // 计算陀螺仪
    lsm6dsdtr->data.gyro.y = (int16_t)((gyro_y_h << 8) | gyro_y_l);
    lsm6dsdtr->data.gyro.z = (int16_t)((gyro_z_h << 8) | gyro_z_l);
    lsm6dsdtr->data.accel.x = (int16_t)((accel_x_h << 8) | accel_x_l); // 计算加速度计
    lsm6dsdtr->data.accel.y = (int16_t)((accel_y_h << 8) | accel_y_l);
    lsm6dsdtr->data.accel.z = (int16_t)((accel_z_h << 8) | accel_z_l);
    lsm6dsdtr->data.angle.x = atan2(lsm6dsdtr->data.accel.y, lsm6dsdtr->data.accel.z) * (180.0 / M_PI); // 计算角度
    lsm6dsdtr->data.angle.y = atan2(-lsm6dsdtr->data.accel.x, sqrt(lsm6dsdtr->data.accel.y * lsm6dsdtr->data.accel.y + lsm6dsdtr->data.accel.z * lsm6dsdtr->data.accel.z)) * (180.0 / M_PI);

    // 计算角度（通过角速度积分）
    float dt = (current_timestamp - lsm6dsdtr->data.timestamp) / 1000000.0; // 时间差，单位为秒

    // 使用互补滤波器融合加速度计和陀螺仪的数据
    float accel_angle_z = atan2(lsm6dsdtr->data.accel.x, sqrt(lsm6dsdtr->data.accel.y * lsm6dsdtr->data.accel.y + lsm6dsdtr->data.accel.z * lsm6dsdtr->data.accel.z)) * (180.0 / M_PI);
    lsm6dsdtr->data.angle_z_accum += ((lsm6dsdtr->data.gyro.z - lsm6dsdtr->cfg.bias) * lsm6dsdtr->data.gyro_sensitivity) * dt;
    lsm6dsdtr->data.angle.z = lsm6dsdtr->cfg.filter_alpha * lsm6dsdtr->data.angle_z_accum + (1 - lsm6dsdtr->cfg.filter_alpha) * accel_angle_z;

    // 更新时间戳
    lsm6dsdtr->data.timestamp = current_timestamp;

    // 发布数据更新
    dds_publish(lsm6dsdtr, &lsm6dsdtr->UPDATE, NULL);
}

// 打印所有参数
void lsm6dsdtr_args_print(lsm6dsdtr_t *lsm6dsdtr)
{
    ASSERT(lsm6dsdtr != NULL);

    // 打印参数
    INFO("%s args:\r\n"
         "ctrl1_xl: 0x%02X\r\n"
         "ctrl2_g: 0x%02X\r\n"
         "ctrl3_c: 0x%02X\r\n"
         "ctrl4_c: 0x%02X\r\n"
         "ctrl5_c: 0x%02X\r\n"
         "ctrl6_c: 0x%02X\r\n"
         "ctrl7_g: 0x%02X\r\n"
         "ctrl8_xl: 0x%02X\r\n"
         "bias: 3.02f\r\n"
         "filter_alpha: %3.02f\r\n",
         lsm6dsdtr->cfg.name,
         lsm6dsdtr->cfg.config.ctrl1_xl,
         lsm6dsdtr->cfg.config.ctrl2_g,
         lsm6dsdtr->cfg.config.ctrl3_c,
         lsm6dsdtr->cfg.config.ctrl4_c,
         lsm6dsdtr->cfg.config.ctrl5_c,
         lsm6dsdtr->cfg.config.ctrl6_c,
         lsm6dsdtr->cfg.config.ctrl7_g,
         lsm6dsdtr->cfg.config.ctrl8_xl,
         lsm6dsdtr->cfg.bias,
         lsm6dsdtr->cfg.filter_alpha);
}

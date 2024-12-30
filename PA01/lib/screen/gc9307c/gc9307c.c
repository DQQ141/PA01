#include "./gc9307c.h"

// 153600, 65535
// 获取能被除数整除的最大值
static uint32_t get_max_divisible(uint32_t dividend, uint16_t divisor)
{
    if (divisor == 0)
    {
        ERROR("divisor is 0");
        return 0; // 返回一个错误值
    }
    uint32_t max_divisible = divisor;

    while ((dividend % max_divisible) != 0 && max_divisible != 0)
    {
        max_divisible--;
    }

    // 计算刚好能被除数整除的最大值
    return max_divisible;
}

void _reg_init(gc9307c_t *lcd)
{
    // 发送初始化序列，开始初始化
    lcd->ops.write_command(0xFE); // 发送初始化序列开始命令
    lcd->ops.write_command(0xEF); // 发送初始化序列开始命令

    // 设置内存访问控制
    lcd->ops.write_command(0x36);              // 发送内存访问控制命令
    lcd->ops.write_data((uint8_t[]){0x48}, 1); // 设置内存访问方向为水平方向

    // 设置颜色模式
    lcd->ops.write_command(0x3A);              // 发送颜色模式设置命令
    lcd->ops.write_data((uint8_t[]){0x05}, 1); // 设置颜色模式为16位色

    // 设置显示控制参数
    lcd->ops.write_command(0x86);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x98}, 1); // 设置显示控制参数
    lcd->ops.write_command(0x89);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x33}, 1); // 设置显示控制参数
    lcd->ops.write_command(0x8B);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x80}, 1); // 设置显示控制参数
    lcd->ops.write_command(0x8D);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x33}, 1); // 设置显示控制参数
    lcd->ops.write_command(0x8E);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x0F}, 1); // 设置显示控制参数

    lcd->ops.write_command(0xEC);                          // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x33, 0x07, 0x00}, 3); // 设置显示控制参数
    lcd->ops.write_command(0xED);                          // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x18, 0x08}, 2);       // 设置显示控制参数

    lcd->ops.write_command(0xE8);                    // 发送其他显示参数设置命令
    lcd->ops.write_data((uint8_t[]){0x12, 0x00}, 2); // 设置其他显示参数（改动：0x01, 0x00 -> 0x12, 0x00）

    // 发送结束初始化序列命令
    lcd->ops.write_command(0xFF);              // 发送结束初始化序列命令
    lcd->ops.write_data((uint8_t[]){0x62}, 1); // 设置结束初始化序列参数

    // 设置显示控制参数
    lcd->ops.write_command(0x99);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x3E}, 1); // 设置显示控制参数
    lcd->ops.write_command(0x9D);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x4B}, 1); // 设置显示控制参数
    lcd->ops.write_command(0x98);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x3E}, 1); // 设置显示控制参数
    lcd->ops.write_command(0x9C);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x4B}, 1); // 设置显示控制参数
    lcd->ops.write_command(0xC3);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x39}, 1); // 设置显示控制参数（改动：0x40 - 8 -> 0x39）
    lcd->ops.write_command(0xC4);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x29}, 1); // 设置显示控制参数（改动：0x30 + 8 -> 0x29）
    lcd->ops.write_command(0xC9);              // 发送显示控制参数设置命令
    lcd->ops.write_data((uint8_t[]){0x09}, 1); // 设置显示控制参数

    // 设置Gamma校正参数
    lcd->ops.write_command(0xF0);                                            // 发送Gamma校正参数设置命令
    lcd->ops.write_data((uint8_t[]){0x0B, 0x0C, 0x07, 0x07, 0x05, 0x22}, 6); // 设置Gamma校正参数（改动：0x17 -> 0x22）
    lcd->ops.write_command(0xF1);                                            // 发送Gamma校正参数设置命令
    lcd->ops.write_data((uint8_t[]){0x42, 0x78, 0x56, 0x1E, 0x1A, 0x95}, 6); // 设置Gamma校正参数（改动：0x3A, 0x78, 0x96, 0x1E, 0x1A, 0x95 -> 0x42, 0x78, 0x56, 0x1E, 0x1A, 0x95）
    lcd->ops.write_command(0xF2);                                            // 发送Gamma校正参数设置命令
    lcd->ops.write_data((uint8_t[]){0x0B, 0x0C, 0x07, 0x07, 0x05, 0x44}, 6); // 设置Gamma校正参数（改动：0x39 -> 0x44）
    lcd->ops.write_command(0xF3);                                            // 发送Gamma校正参数设置命令
    lcd->ops.write_data((uint8_t[]){0x56, 0x92, 0x30, 0x31, 0x35, 0x7F}, 6); // 设置Gamma校正参数（改动：0x4E, 0x92, 0x70, 0x31, 0x35, 0x7F -> 0x56, 0x92, 0x30, 0x31, 0x35, 0x7F）

    // 设置显示方向
    lcd->ops.write_command(GC9307C_CMD_MADCTL);
    lcd->ops.write_data((uint8_t *)&lcd->cfg.direction, 1);
    lcd->ops.write_command(GC9307C_CMD_TEON); // 开启TE信号输出

    lcd->ops.write_command(0x11); // 退出睡眠模式
    lcd->ops.write_command(0x29); // 开启显示
}

// 初始化序列
void gc9307c_reg_init(gc9307c_t *lcd)
{
    if (!MUTEX_LOCK(&lcd->mutex))
    {
        ERROR("[%s] reg_init failed, mutex lock failed.", lcd->cfg.name);
        return;
    }

    _reg_init(lcd);

    MUTEX_UNLOCK(&lcd->mutex);
}

// 进入休眠
void gc9307c_enter_sleep(gc9307c_t *lcd)
{
    if (!lcd->flag.is_inited)
    {
        ERROR("[%s] enter_sleep failed, lcd not inited.", lcd->cfg.name);
        return;
    }

    // 锁定互斥量以确保线程安全
    if (!MUTEX_LOCK(&lcd->mutex))
    {
        ERROR("[%s] enter_sleep failed, mutex lock failed.", lcd->cfg.name);
        return;
    }

    lcd->ops.write_command(0x28);
    lcd->ops.delay();
    lcd->ops.write_command(0x10);
    lcd->ops.delay();

    // 解锁互斥量
    MUTEX_UNLOCK(&lcd->mutex);
}

// 退出睡眠模式
void gc9307c_exit_sleep(gc9307c_t *lcd)
{
    if (!lcd->flag.is_inited)
    {
        ERROR("[%s] exit_sleep failed, lcd not inited.", lcd->cfg.name);
        return;
    }

    // 锁定互斥量以确保线程安全
    if (!MUTEX_LOCK(&lcd->mutex))
    {
        ERROR("[%s] exit_sleep failed, mutex lock failed.", lcd->cfg.name);
        return;
    }

    lcd->ops.write_command(0x11);
    lcd->ops.delay();
    lcd->ops.write_command(0x29);

    // 解锁互斥量
    MUTEX_UNLOCK(&lcd->mutex);
}

// 初始化屏幕
void gc9307c_init(gc9307c_t *lcd)
{
    // 检查参数
    ASSERT(lcd != NULL);
    ASSERT(lcd->cfg.name != NULL);
    ASSERT(lcd->cfg.buff != NULL);
    ASSERT(lcd->cfg.width != 0);
    ASSERT(lcd->cfg.height != 0);
    ASSERT(lcd->cfg.direction != 0);
    ASSERT(lcd->ops.init != NULL);
    ASSERT(lcd->ops.reset != NULL);
    ASSERT(lcd->ops.write_command != NULL);
    ASSERT(lcd->ops.write_data != NULL);
    ASSERT(lcd->ops.delay != NULL);

    // 初始化屏幕
    lcd->ops.init();

    // 互斥锁
    if (!MUTEX_LOCK(&lcd->mutex))
    {
        ERROR("[%s] mutex lock failed.", lcd->cfg.name);
        return;
    }

    // 重置屏幕
    lcd->ops.reset();
    _reg_init(lcd); // 发送初始化序列

    lcd->ops.write_command(0x11);
    lcd->ops.delay(); // 延时等待复位完成
    lcd->ops.write_command(0x29);

    switch (lcd->cfg.direction)
    {
    case GC9307C_DIR_VERTICAL:
    case GC9307C_DIR_VERTICAL_FLIP_Y:
    case GC9307C_DIR_VERTICAL_FLIP_X:
    case GC9307C_DIR_VERTICAL_FLIP_XY:
        lcd->height = lcd->cfg.width;
        lcd->width = lcd->cfg.height;
        break;
    case GC9307C_DIR_HORIZONTAL:
    case GC9307C_DIR_HORIZONTAL_FLIP_X:
    case GC9307C_DIR_HORIZONTAL_FLIP_Y:
    case GC9307C_DIR_HORIZONTAL_FLIP_XY:
        lcd->height = lcd->cfg.height;
        lcd->width = lcd->cfg.width;
        break;
    default:
        ERROR("[%s] direction error.", lcd->cfg.name);
        ASSERT(0);
        break;
    }

    // 清屏
    lcd->ops.write_command(GC9307C_CMD_CASET); // 设置列地址
    lcd->ops.write_data((uint8_t[]){0, 0, (lcd->width - 1) >> 8, (lcd->width - 1) & 0xFF}, 4);

    lcd->ops.write_command(GC9307C_CMD_PASET); // 设置行地址
    lcd->ops.write_data((uint8_t[]){0, 0, (lcd->height - 1) >> 8, (lcd->height - 1) & 0xFF}, 4);

    lcd->ops.write_command(GC9307C_CMD_RAMWR); // 开始写数据
    uint32_t length = lcd->width * lcd->height;
    for (int i = 0; i < length; i++)
    {
        lcd->cfg.buff[i] = GC9307C_COLOR_BLACK;
    }
    lcd->ops.write_data((uint8_t *)lcd->cfg.buff, length * 2);
    MUTEX_UNLOCK(&lcd->mutex);

    // 初始化完毕
    lcd->flag.is_inited = true;
    INFO("[%s] init success.", lcd->cfg.name);
}

// 帧数计数
void gc9307c_frame_count_hook(gc9307c_t *lcd, uint64_t timestamp)
{
    if (!lcd->flag.is_inited)
    {
        return;
    }

    // 计算帧间隔
    lcd->frame_gap = timestamp - lcd->timestamp;

    // 更新时间戳
    lcd->timestamp = timestamp;
}

/**
 * @brief 设置绘图窗口
 *
 * @param lcd 指向gc9307c_t结构体的指针
 * @param x1 起始X坐标
 * @param y1 起始Y坐标
 * @param x2 结束X坐标
 * @param y2 结束Y坐标
 */
static void gc9307c_set_window(gc9307c_t *lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (!lcd->flag.is_inited)
    {
        ERROR("[%s] set_window failed, lcd not inited.", lcd->cfg.name);
        return;
    }

    lcd->ops.write_command(GC9307C_CMD_CASET); // 设置列地址
    lcd->ops.write_data((uint8_t[]){x1 >> 8, x1 & 0xFF, x2 >> 8, x2 & 0xFF}, 4);

    lcd->ops.write_command(GC9307C_CMD_PASET); // 设置行地址
    lcd->ops.write_data((uint8_t[]){y1 >> 8, y1 & 0xFF, y2 >> 8, y2 & 0xFF}, 4);

    lcd->ops.write_command(GC9307C_CMD_RAMWR); // 开始写数据
}

// 清屏
void gc9307c_clear(gc9307c_t *lcd, uint16_t color)
{
    if (!lcd->flag.is_inited)
    {
        ERROR("[%s] clear failed, lcd not inited.", lcd->cfg.name);
        return;
    }

    // 锁定互斥量以确保线程安全
    if (!MUTEX_LOCK(&lcd->mutex))
    {
        ERROR("[%s] clear failed, mutex lock failed.", lcd->cfg.name);
        return;
    }

    gc9307c_set_window(lcd, 0, 0, lcd->width - 1, lcd->height - 1);
    uint32_t length = lcd->width * lcd->height;
    for (int i = 0; i < length; i++)
    {
        lcd->cfg.buff[i] = color;
    }
    lcd->ops.write_data((uint8_t *)lcd->cfg.buff, length * 2);
    MUTEX_UNLOCK(&lcd->mutex);
}

// 将指定区域的像素数据写入到GC9307C LCD显示屏中
void gc9307c_write(gc9307c_t *lcd, uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t *buff, uint32_t length)
{
    if (!lcd->flag.is_inited)
    {
        ERROR("[%s] write failed, lcd not inited.", lcd->cfg.name);
        return;
    }

    // 确保写入区域在LCD的范围内
    if (x >= lcd->width || y >= lcd->height || x2 >= lcd->width || y2 >= lcd->height || x > x2 || y > y2)
    {
        // 如果区域无效，直接返回
        return;
    }

    // 锁定互斥量以确保线程安全
    if (!MUTEX_LOCK(&lcd->mutex))
    {
        ERROR("[%s] write failed, mutex lock failed.", lcd->cfg.name);
        return;
    }

    // 设置写入窗口
    gc9307c_set_window(lcd, x, y, x2, y2);

    // 计算写入区域的总像素数
    uint32_t total_pixels = (x2 - x + 1) * (y2 - y + 1);

    // 检查缓冲区长度是否足够
    if (length < total_pixels)
    {
        // 如果缓冲区长度不足，解锁互斥量并返回
        MUTEX_UNLOCK(&lcd->mutex);
        return;
    }

    // 将缓冲区中的数据写入LCD
    lcd->ops.write_data((uint8_t *)buff, total_pixels * sizeof(uint16_t));

    // 解锁互斥量
    MUTEX_UNLOCK(&lcd->mutex);
}

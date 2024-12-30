#include "./flash.h"

// 初始化FLASH驱动
void flash_init(flash_t *flash)
{
    // 参数检查
    ASSERT(flash != NULL);
    ASSERT(flash->cfg.name != NULL);         // 芯片名称
    ASSERT(flash->cfg.write_max_size > 0);   // 单次写入最大值
    ASSERT(flash->cfg.erase_min_size > 0);   // 单次擦除最小值
    ASSERT(flash->cfg.erase_block_size > 0); // 单次擦除大块数据大小
    ASSERT(flash->cfg.chip_size > 0);        // 总容量
    ASSERT(flash->ops.init != NULL);         // 初始化函数指针
    ASSERT(flash->ops.erase != NULL);        // 擦除扇区函数指针
    ASSERT(flash->ops.erase_block != NULL);  // 擦除大块数据函数指针
    ASSERT(flash->ops.read != NULL);         // 读取函数指针
    ASSERT(flash->ops.write != NULL);        // 写入函数指针

    // 调用初始化函数
    flash->ops.init();
    flash->flag.is_inited = true;
    dds_publish(flash, &flash->INIT, NULL);
}

// 初始化FLASH驱动
void flash_deinit(flash_t *flash)
{
    ASSERT(flash != NULL);

    MUTEX_LOCK(&flash->mutex);
    flash->flag.is_inited = false;
    MUTEX_UNLOCK(&flash->mutex);

    dds_publish(flash, &flash->DEINIT, NULL);
}

// 擦除FLASH芯片中的数据
void flash_erase(flash_t *flash, uint32_t address, uint32_t length)
{
    ASSERT(flash != NULL);
    ASSERT(length != 0);
    ASSERT(address + length <= flash->cfg.chip_size);

    if (!flash->flag.is_inited)
    {
        ERROR("flash_erase failed, flash not inited.\r\n");
        return;
    }

    // 获取互斥锁
    MUTEX_LOCK(&flash->mutex);

    // 完成擦除操作
    uint32_t erase_min_size = flash->cfg.erase_min_size;
    uint32_t erase_block_size = flash->cfg.erase_block_size;
    uint32_t end_address = address + length;

    // 计算起始地址和结束地址所在的块边界
    uint32_t start_block_address = (address / erase_block_size) * erase_block_size;
    uint32_t end_block_address = ((end_address + erase_block_size - 1) / erase_block_size) * erase_block_size;

    // 优先使用大范围擦除函数
    for (uint32_t addr = start_block_address; addr < end_block_address; addr += erase_block_size)
    {
        if (addr + erase_block_size > end_block_address)
        {
            // 剩余区域不足一个块大小，使用小范围擦除
            for (uint32_t sub_addr = addr; sub_addr < end_block_address; sub_addr += erase_min_size)
            {
                flash->ops.erase(sub_addr);
            }
        }
        else
        {
            // 使用大范围擦除
            flash->ops.erase_block(addr);
        }
    }

    // 释放互斥锁
    MUTEX_UNLOCK(&flash->mutex);

    void *args[] = {&address, &length};
    dds_publish(flash, &flash->ERASE, args);
}

// 向FLASH芯片写入数据
void flash_write(flash_t *flash, uint32_t address, uint8_t *buff, uint32_t length)
{
    // 确保写入地址和长度在芯片范围内
    ASSERT(flash != NULL);
    ASSERT(buff != NULL);
    ASSERT(address + length <= flash->cfg.chip_size);

    if (!flash->flag.is_inited)
    {
        ERROR("flash_write failed, flash not inited.\r\n");
        return;
    }

    // 获取互斥锁
    MUTEX_LOCK(&flash->mutex);

    // 写入数据
    uint32_t write_size = flash->cfg.write_max_size;
    uint32_t end_address = address + length;

    // 一次性擦除需要写入的区域
    flash_erase(flash, address, length);

    // 处理跨区写入
    uint32_t erase_block_size = flash->cfg.erase_block_size;
    uint32_t current_block_start = (address / erase_block_size) * erase_block_size;
    uint32_t next_block_start = current_block_start + erase_block_size;

    for (uint32_t addr = address; addr < end_address; addr += write_size)
    {
        if (addr + write_size > next_block_start)
        {
            // 如果写入操作跨区，则调整写入大小以避免跨区
            write_size = next_block_start - addr;
        }

        if (addr + write_size > end_address)
        {
            // 如果剩余数据不足一个写入大小，调整写入大小
            write_size = end_address - addr;
        }

        flash->ops.write(addr, buff + (addr - address), write_size);

        // 更新当前块和下一个块的起始地址
        if (addr + write_size == next_block_start)
        {
            current_block_start = next_block_start;
            next_block_start += erase_block_size;
        }
    }

    // 释放互斥锁
    MUTEX_UNLOCK(&flash->mutex);

    void *args[] = {&address, &buff, &length};
    dds_publish(flash, &flash->WRITE, args);
}

// 从FLASH芯片读取数据
void flash_read(flash_t *flash, uint32_t address, uint8_t *buff, uint32_t length)
{
    ASSERT(flash != NULL);
    ASSERT(length != 0);
    ASSERT(buff != NULL);
    ASSERT(address + length <= flash->cfg.chip_size);

    if (!flash->flag.is_inited)
    {
        ERROR("flash_read failed, flash not inited.\r\n");
        return;
    }

    // 获取互斥锁
    MUTEX_LOCK(&flash->mutex);
    flash->ops.read(address, buff, length);

    // 释放互斥锁
    MUTEX_UNLOCK(&flash->mutex);

    void *args[] = {&address, &buff, &length};
    dds_publish(flash, &flash->READ, args);
}

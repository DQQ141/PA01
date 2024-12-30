/**
 * @file sdcard_test.cc
 * @author WittXie
 * @brief SD卡测试
 * @version 0.1
 * @date 2024-10-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"
static void fat32_file_write_test(void)
{
    FIL file;    // 文件对象
    FRESULT res; // 文件操作结果
    UINT bytes_written, bytes_read;
    char write_data[] = "Hello, this is a test message!";
    char file_name[] = "test.txt"; // 文件名

    // 挂载文件系统
    if (f_mount(&SDFatFS, "", 0) == FR_OK)
    {
        // 打开文件(如果不存在则创建)
        res = f_open(&file, file_name, FA_CREATE_ALWAYS | FA_WRITE);
        if (res == FR_OK)
        {
            // 写入数据到文件
            res = f_write(&file, write_data, sizeof(write_data), &bytes_written);
            if (res == FR_OK && bytes_written == sizeof(write_data))
            {
                print("FAT32 写入内容%s到%s成功\r\n", write_data, file_name);
            }
            else
            {
                log_error("FAT32 写入数据失败\r\n");
            }

            // 关闭文件
            f_close(&file);
        }
        else
        {
            log_error("FAT32 打开文件失败\r\n");
        }
    }
    else
    {
        log_error("FAT32 挂载文件系统失败\r\n");
    }
}

static void fat32_file_read_test(void)
{
    FIL file;    // 文件对象
    FRESULT res; // 文件操作结果
    UINT bytes_written, bytes_read;
    char read_data[64];            // 预留读取数据的缓冲区
    char file_name[] = "test.txt"; // 文件名

    // 挂载文件系统
    if (f_mount(&SDFatFS, "", 0) == FR_OK)
    {
        // 再次打开文件进行读取
        res = f_open(&file, file_name, FA_READ);
        if (res == FR_OK)
        {
            // 读取文件数据
            res = f_read(&file, read_data, sizeof(read_data) - 1, &bytes_read);
            if (res == FR_OK)
            {
                read_data[bytes_read] = '\0'; // 添加字符串结束符
                print("FAT32 从%s读取数据成功: %s\r\n", file_name, read_data);
            }
            else
            {
                log_error("FAT32 读取文件失败\r\n");
            }

            // 关闭文件
            f_close(&file);
        }
        else
        {
            log_error("FAT32 打开文件失败\r\n");
        }

        // 卸载文件系统
        f_mount(NULL, "", 0);
    }
    else
    {
        log_error("FAT32 挂载文件系统失败\r\n");
    }
}

static void sdcard_info_test(void)
{
    DWORD fre_clust;
    float total_size_mb, free_size_mb;
    float card_speed_mb_s;

    HAL_SD_CardInfoTypeDef info = {0};
    BSP_SD_GetCardInfo(&info);

    // 打印SD卡信息(注意单位)
    if (info.Class != 0)
    {
        print("SD卡信息:\r\n");
        print("卡版本: %d\r\n", info.CardVersion);
        print("卡类别: %d\r\n", info.Class);

        // 块大小 (单位: KB)
        print("块大小: %d KB\r\n", info.LogBlockSize);

        // 块数
        print("块数: %d\r\n", info.LogBlockNbr);

        // 卡容量 (单位: MB)
        uint32_t log_card_capacity_mb = (info.LogBlockNbr * info.LogBlockSize) / (1024 * 1024);
        print("卡逻辑容量: %d MB\r\n", log_card_capacity_mb);

        // 挂载SDFatFS文件系统
        if (f_mount(&SDFatFS, "", 0) == FR_OK)
        {
            // 获取剩余簇数量
            FATFS *fs = &SDFatFS;
            if (f_getfree("", &fre_clust, &fs) == FR_OK)
            {
                // 计算总容量和剩余容量
                total_size_mb = (SDFatFS.n_fatent - 2) * SDFatFS.csize / 2.0f / 1024;
                free_size_mb = fre_clust * SDFatFS.csize / 2.0f / 1024;

                print("FAT32 总容量: %5.02f MB\r\n", total_size_mb);
                print("FAT32 剩余容量: %5.02f MB\r\n", free_size_mb);
            }
            else
            {
                log_error("FAT32 获取剩余容量失败\r\n");
            }
            // 卸载文件系统
            f_mount(fs, "", 0);
        }
        else
        {
            log_error("FAT32 挂载文件系统失败\r\n");
        }
    }
    else
    {
        log_error("FAT32 未检测到SD卡\r\n");
    }
}

// 测试任务
static void sdcard_test_entry(void *args)
{
    os_sleep(500);
    log_info("sdcard test entry.");

    // 测试SD卡
    sdcard_info_test();

    // 测试文件系统
    fat32_file_write_test(); // 写入文件
    fat32_file_read_test();  // 读取文件

    log_info("sdcard test end.");
    os_return;
}

// 初始化测试
static void sdcard_test(void)
{
    os_task_create(sdcard_test_entry, "sdcard_test", NULL, OS_PRIORITY_APP, OS_TASK_STACK_MIN + 2048);
}

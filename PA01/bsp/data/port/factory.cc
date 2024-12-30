#include "./../data_bsp.h"
#include "ff.h" // FatFs 头文件

static const char s_factory_data_bin_file_name[] = "data/factory_data.bin"; // 文件名
static FIL s_factory_file;                                                  // 文件对象
static FRESULT s_factory_res;                                               // 文件操作结果
static UINT s_factory_bytes;

static void factory_data_init(void)
{
}

static void factory_data_write(uint8_t *buff, uint32_t length)
{
    if (f_mount(&SDFatFS, "", 0) != FR_OK)
    {
        log_error("mount s_factory_file system failed.");
        return;
    }

    // 确保路径存在
    char path[256];
    strncpy(path, s_factory_data_bin_file_name, sizeof(path));
    char *dir = strrchr(path, '/');
    if (dir)
    {
        *dir = '\0';
        f_mkdir(path);
    }

    // 打开文件（如果不存在则创建，存在则覆盖）
    s_factory_res = f_open(&s_factory_file, s_factory_data_bin_file_name, FA_CREATE_ALWAYS | FA_WRITE);
    if (s_factory_res == FR_OK)
    {
        // 写入数据到文件
        s_factory_res = f_write(&s_factory_file, buff, length, &s_factory_bytes);
        if (!(s_factory_res == FR_OK))
        {
            // 写入成功
            log_error("write 'factory_data.bin' failed. Error: %s", f_error_to_string(s_factory_res));
        }

        // 关闭文件
        f_close(&s_factory_file);
    }
    else
    {
        log_error("open 'factory_data.bin' failed. Error: %s", f_error_to_string(s_factory_res));
    }

    // 卸载文件系统
    f_mount(NULL, "", 0);
}

static void factory_data_read(uint8_t *buff, uint32_t length)
{
    if (f_mount(&SDFatFS, "", 0) != FR_OK)
    {
        log_error("mount s_factory_file system failed.");
        return;
    }

    // 确保路径存在
    char path[256];
    strncpy(path, s_factory_data_bin_file_name, sizeof(path));
    char *dir = strrchr(path, '/');
    if (dir)
    {
        *dir = '\0';
        f_mkdir(path);
    }

    // 打开文件以进行读取
    s_factory_res = f_open(&s_factory_file, s_factory_data_bin_file_name, FA_OPEN_ALWAYS | FA_READ);
    if (s_factory_res == FR_OK)
    {
        // 读取文件数据
        s_factory_res = f_read(&s_factory_file, buff, length, &s_factory_bytes);
        if (!(s_factory_res == FR_OK))
        {
            log_error("read 'factory_data.bin' failed. Error: %s", f_error_to_string(s_factory_res));
        }

        // 关闭文件
        f_close(&s_factory_file);
    }
    else
    {
        log_error("open 'factory_data.bin' failed. Error: %s", f_error_to_string(s_factory_res));
    }

    // 卸载文件系统
    f_mount(NULL, "", 0);
}

static uint16_t factory_data_check(uint8_t *buff, uint32_t length)
{
    // 检查完整性
    return crc_calculate(&g_crc_ccitt, buff, length);
}

data_t g_data_factory = {
    .cfg = {
        .name = "g_data_factory",
    },
    .ops = {
        .init = factory_data_init,
        .read = factory_data_read,
        .write = factory_data_write,
        .check = factory_data_check,
    },
};

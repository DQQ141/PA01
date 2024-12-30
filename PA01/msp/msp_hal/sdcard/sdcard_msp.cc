/**
 * @file sdcard_msp.cc
 * @author WittXie
 * @brief SDCARD初始化
 * @version 0.1
 * @date 2024-10-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../msp_env.h"

const char *f_error_to_string(FRESULT res)
{
    switch (res)
    {
    case FR_OK:
        return "Succeeded";
    case FR_DISK_ERR:
        return "A hard error occurred in the low level disk I/O layer";
    case FR_INT_ERR:
        return "Assertion failed";
    case FR_NOT_READY:
        return "The physical drive cannot work";
    case FR_NO_FILE:
        return "Could not find the file";
    case FR_NO_PATH:
        return "Could not find the path";
    case FR_INVALID_NAME:
        return "The path name format is invalid";
    case FR_DENIED:
        return "Access denied due to prohibited access or directory full";
    case FR_EXIST:
        return "Access denied due to prohibited access";
    case FR_INVALID_OBJECT:
        return "The file/directory object is invalid";
    case FR_WRITE_PROTECTED:
        return "The physical drive is write protected";
    case FR_INVALID_DRIVE:
        return "The logical drive number is invalid";
    case FR_NOT_ENABLED:
        return "The volume has no work area";
    case FR_NO_FILESYSTEM:
        return "There is no valid FAT volume";
    case FR_MKFS_ABORTED:
        return "The f_mkfs() aborted due to any problem";
    case FR_TIMEOUT:
        return "Could not get a grant to access the volume within defined period";
    case FR_LOCKED:
        return "The operation is rejected according to the file sharing policy";
    case FR_NOT_ENOUGH_CORE:
        return "LFN working buffer could not be allocated";
    case FR_TOO_MANY_OPEN_FILES:
        return "Number of open files > FF_FS_LOCK";
    case FR_INVALID_PARAMETER:
        return "Given parameter is invalid";
    default:
        return "Unknown error";
    }
}

uint8_t sdcard_msp_init()
{
    SD_HandleTypeDef *hsd = &hsd1;
    // 重置SD卡
    HAL_SD_DeInit(hsd);
    HAL_Delay(100);

    // 初始化SD卡
    uint8_t sd_state = MSD_OK;
    /* HAL SD initialization */
    sd_state = HAL_SD_Init(hsd);
    /* Configure SD Bus width (4 bits mode selected) */
    if (sd_state == MSD_OK)
    {
        /* Enable wide operation */
        if (HAL_SD_ConfigWideBusOperation(hsd, SDMMC_BUS_WIDE_4B) != HAL_OK)
        {
            sd_state = MSD_ERROR;
        }
    }
    return sd_state;
}

// 重写FAT32的SD初始化函数
uint8_t BSP_SD_Init(void)
{
    return sdcard_msp_init();
}

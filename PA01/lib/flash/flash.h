/**
 * @file flash.h
 * @author WittXie
 * @brief FLASH 驱动
 * @version 0.1
 * @date 2024-09-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

// 依赖
#include "./../dds/dds.h"       // 订阅机制
#include "./../string/string.h" // countof

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

#ifndef ERROR
#define ERROR(_format, ...) ((void)0)
#endif

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef MUTEX_LOCK
#define MUTEX_LOCK(_mutex) ((bool)true)
#define MUTEX_UNLOCK(_mutex) ((void)0)
#endif

typedef struct __flash
{
    // 参数
    struct
    {
        char *name;                // 芯片名称
        uint32_t write_max_size;   // 单次写入最大值
        uint32_t erase_min_size;   // 单次擦除最小值
        uint32_t erase_block_size; // 单次擦除大块数据大小
        uint32_t chip_size;        // 总容量
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);
        void (*erase)(uint32_t address);                                 // 擦除扇区 (参数为扇区地址)
        void (*erase_block)(uint32_t address);                           // 擦除大块数据 (参数为块地址)
        void (*read)(uint32_t address, uint8_t *buff, uint32_t length);  // 读取
        void (*write)(uint32_t address, uint8_t *buff, uint32_t length); // 写入 (一般不超过页大小)
    } ops;

    // 标志
    union
    {
        uint8_t value;
        struct
        {
            bool is_inited : 1; // 是否已初始化
        };
    } flag;

    void *mutex; // 互斥锁指针

    dds_topic_t INIT;   // 初始化
    dds_topic_t DEINIT; // 反初始化后
    dds_topic_t ERASE;  // 擦除
    dds_topic_t READ;   // 读取
    dds_topic_t WRITE;  // 写入
} flash_t;

/**
 * @brief 初始化 FLASH 驱动
 *
 * @param flash 设备指针
 */
void flash_init(flash_t *flash);

/**
 * @brief 销毁 FLASH 驱动
 *
 * @param flash 设备指针
 */
void flash_deinit(flash_t *flash);

/**
 * @brief 写入数据
 *
 * @param flash 设备指针
 * @param address flash芯片内部地址, 比如0x1000
 * @param buff 要写入的数据
 * @param length 要写入的数据长度
 * @note 写入前会自动擦除需要改写的扇区,擦除前会保存不需要改写的数据,擦除后还原这些数据
 */
void flash_write(flash_t *flash, uint32_t address, uint8_t *buff, uint32_t length);

/**
 * @brief 读取数据
 *
 * @param flash 设备指针
 * @param address flash芯片内部地址, 比如0x1000
 * @param buff 要读取的数据
 * @param length 要读取的数据长度
 */
void flash_read(flash_t *flash, uint32_t address, uint8_t *buff, uint32_t length);

/**
 * @brief 擦除数据
 *
 * @param flash 设备指针
 * @param address flash芯片内部地址, 比如0x1000
 * @param length 要擦除的数据长度
 * @note 擦除前会保存不需要改写的数据,擦除后还原这些数据
 */
void flash_erase(flash_t *flash, uint32_t address, uint32_t length);

/**
 * @brief 设备是否初始化
 *
 * @param flash 设备指针
 * @return bool true为已初始化，false为未初始化
 */
#define flash_is_inited(flash) ((flash)->flag.is_inited)

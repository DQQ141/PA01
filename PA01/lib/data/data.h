/**
 * @file data.h
 * @author WittXie
 * @brief 数据管理
 * @version 0.1
 * @date 2024-11-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

// 依赖
#include "./../dds/dds.h"   // 订阅机制
#include "./../time/time.h" // 时间

// 通用接口
#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

#ifndef ERROR
#define ERROR(_format, ...) ((void)0)
#endif

#ifndef MALLOC
#error "pleasae define MALLOC and FREE"
#define MALLOC(_size) malloc(_size)
#define FREE(_pv) free(_pv)
#endif

#ifndef MUTEX_LOCK
#define MUTEX_LOCK(_mutex) ((bool)true)
#define MUTEX_UNLOCK(_mutex) ((void)0)
#endif

#pragma pack(4)
typedef struct __data
{
    struct
    {
        const char *name;
        void *user_data;
        uint32_t length; // 数据长度
    } cfg;

    struct
    {
        void (*init)(void);
        uint16_t (*check)(uint8_t *buff, uint32_t length);
        void (*read)(uint8_t *buff, uint32_t length);
        void (*write)(uint8_t *buff, uint32_t length);
    } ops;

    uint8_t *cache; // 缓存
    void *mutex;    // 互斥锁
} data_t;
#pragma pack()

/**
 * @brief 数据初始化
 *
 * @param data 数据
 */
void data_init(data_t *data);

/**
 * @brief 数据保存
 *
 * @param data 数据
 */
void data_save(data_t *data);

/**
 * @brief 数据恢复
 *
 * @param data 数据
 * @return true
 * @return false
 */
void data_load(data_t *data);

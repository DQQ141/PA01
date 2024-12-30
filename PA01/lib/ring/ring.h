/**
 * @file ring.h
 * @author WittXie
 * @brief 环形队列；支持多生产者和多消费者；支持线程安全；支持读/写锁独立; 支持边读边写; 数据版本管理 防ABA
 * @version 0.1
 * @date 2024-08-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef ERROR
#define ERROR(_format, ...) ((void)0)
#endif

#ifndef MALLOC
#define MALLOC(_size) malloc(_size)
#define FREE(_pv) free(_pv)
#endif

#ifndef MUTEX_LOCK
#define MUTEX_LOCK(_mutex) ((bool)true)
#define MUTEX_UNLOCK(_mutex) ((void)0)
#endif

/**
 * @brief 环形队列结构体
 */
typedef struct __ring
{
    uint8_t *buff;    /**< 缓冲区指针 */
    uint32_t size;    /**< 缓冲区大小 */
    uint32_t head;    /**< 读位置 */
    uint32_t tail;    /**< 写位置 */
    void *mutex;      /**< 读锁 */
    uint32_t version; /**< 版本号，用于避免ABA问题 */
} ring_t;

/**
 * @brief 环形队列是否已满
 *
 * @param ring 环形队列指针
 * @return true: 环形队列已满
 * @return flase: 环形队列未满
 */
#define ring_is_full(_ring) ((((_ring)->tail + 1) % (_ring)->size) == (_ring)->head)

/**
 * @brief 环形队列是否为空
 *
 * @param ring 环形队列指针
 * @return true: 环形队列为空
 * @return flase: 环形队列不为空
 */
#define ring_is_empty(_ring) ((_ring)->head == (_ring)->tail)

/**
 * @brief 初始化环形队列
 *
 * @param ring 环形队列指针
 * @param buff_size 缓冲区大小
 * @return true: 初始化成功, false: 初始化失败
 */
bool ring_init(ring_t *ring, uint32_t buff_size);

/**
 * @brief 销毁环形队列
 *
 * @param ring 环形队列指针
 */
void ring_deinit(ring_t *ring);

/**
 * @brief 写入数据到环形队列
 *
 * @param ring 环形队列指针
 * @param data 数据指针
 * @param length 数据长度
 * @return uint32_t 实际写入的字节数
 */
uint32_t ring_enqueue(ring_t *ring, const uint8_t *data, uint32_t length);

/**
 * @brief 从环形队列读取数据
 *
 * @param ring 环形队列指针
 * @param data 数据指针
 * @param length 数据长度
 * @return uint32_t 实际读取的字节数
 */
uint32_t ring_dequeue(ring_t *ring, uint8_t *data, uint32_t length);

/**
 * @brief 丢弃环形队列中的数据
 *
 * @param ring 环形队列指针
 * @param length 丢弃的数据长度
 * @return uint32_t 实际丢弃的字节数
 */
uint32_t ring_discard(ring_t *ring, uint32_t length);

/**
 * @brief 获取环形队列的可用空间
 *
 * @param ring 环形队列指针
 * @return uint32_t 可用空间大小
 */
uint32_t ring_remain_size(ring_t *ring);

/**
 * @brief 获取环形队列的数据量
 *
 * @param ring 环形队列指针
 * @return uint32_t 数据量大小
 */
uint32_t ring_data_size(ring_t *ring);

/**
 * @brief 重置环形队列
 *
 * @param ring 环形队列指针
 */
void ring_clear(ring_t *ring);

/**
 * @brief 窥视环形队列的数据，不改变读指针
 *
 * @param ring 环形队列指针
 * @param data 数据指针
 * @param length 数据长度
 * @return uint32_t 实际窥视的字节数
 */
uint32_t ring_peek(ring_t *ring, uint8_t *data, uint32_t length);

/**
 * @brief 从环形队列指定位置删除数据
 *
 * @param ring 环形队列指针
 * @param posi 插入位置
 * @param data_length 数据长度
 * @return uint32_t 实际删除的字节数
 */
uint32_t ring_delete(ring_t *ring, uint32_t index, uint32_t data_length);

/**
 * @brief 在环形队列指定位置插入数据
 *
 * @param ring 环形队列指针
 * @param index 插入起始位置
 * @param data 数据指针
 * @param data_length 数据长度
 * @return uint32_t 实际插入的字节数
 */
uint32_t ring_insert(ring_t *ring, uint32_t index, const uint8_t *data, uint32_t data_length);

/**
 * @brief 在环形队列指定位置提出数据
 *
 * @param ring 环形队列指针
 * @param index 提取起始位置
 * @param data 数据指针
 * @param length 数据长度
 * @return uint32_t 实际提出的字节数
 */
uint32_t ring_takeout(ring_t *ring, uint32_t index, uint8_t *data, uint32_t length);

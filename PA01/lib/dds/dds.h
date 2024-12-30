/**
 * @file dds.h
 * @author WittXie
 * @brief Data Distribution Service 数据分发服务
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "./../list/list.h"
#include "./../time/time.h"

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
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

#ifndef TIMESTAMP_US_GET
#error "please define TIMESTAMP_US_GET"
#define TIMESTAMP_US_GET() 0
#endif

#define DDS_PRIORITY_SUPER 0x1000
#define DDS_PRIORITY_HIGH 0x4000
#define DDS_PRIORITY_NORMAL 0x8000
#define DDS_PRIORITY_LOW 0xC000
#define DDS_PRIORITY_LOWEST 0xE000

typedef struct __dds_node dds_node_t;
typedef list_t dds_topic_t;

/**
 * @brief DDS节点的回调函数类型定义
 * @param device 设备指针
 * @param topic 主题
 * @param arg 设备传参
 * @param userdata 用户传参
 */
typedef void (*dds_callback_t)(void *device, dds_topic_t *topic, void *arg, void *userdata);

/**
 * @brief DDS任务函数类型定义
 * @param userdata 用户传参
 */
typedef void (*dds_task_fn_t)(void *userdata);

typedef struct __dds_node
{
    list_node_t *list_node;  // 链表表头
    dds_callback_t callback; // 回调函数
    uint16_t priority;       // 优先级
    uint32_t skip_cnt;       // 是否跳过
    void *userdata;          // 用户数据
} dds_node_t;                // dds节点

/**
 * @brief 使用节点指针订阅DDS主题
 * @param topic 指向DDS主题的指针
 * @param dds_node 要订阅的节点的指针
 * @return 成功订阅返回节点指针，失败返回NULL
 */
dds_node_t *dds_subcribe_with_node(dds_topic_t *topic, dds_node_t *dds_node);

/**
 * @brief 使用回调函数和优先级订阅DDS主题
 * @param topic 指向DDS主题的指针
 * @param priority 订阅节点的优先级
 * @param callback 订阅节点的回调函数
 * @param userdata 用户数据
 * @return 成功订阅返回节点指针，失败返回NULL
 */
dds_node_t *dds_subcribe(dds_topic_t *topic, uint16_t priority, dds_callback_t callback, void *userdata);

/**
 * @brief 在指定节点前/后订阅
 *
 * @param topic 指向DDS主题的指针
 * @param target_node 指定节点
 * @param callback 订阅节点的回调函数
 * @param userdata 用户数据
 * @return dds_node_t* 成功订阅返回节点指针，失败返回NULL
 */
dds_node_t *dds_subcribe_before(dds_topic_t *topic, dds_node_t *target_node, dds_callback_t callback, void *userdata);
dds_node_t *dds_subcribe_after(dds_topic_t *topic, dds_node_t *target_node, dds_callback_t callback, void *userdata);

/**
 * @brief 使用节点指针取消DDS主题的订阅
 * @param topic 指向DDS主题的指针
 * @param dds_node 要取消订阅的节点的指针
 */
void dds_unsubcribe_with_node(dds_topic_t *topic, dds_node_t *dds_node);

/**
 * @brief 取消回调函数的全部订阅
 * @param topic 指向DDS主题的指针
 * @param callback 要取消订阅的回调函数
 */
void dds_unsubcribe(dds_topic_t *topic, dds_callback_t callback);

/**
 * @brief 取消DDS主题的所有订阅
 * @param topic 指向DDS主题的指针
 */
void dds_unsubcribe_all(dds_topic_t *topic);

/**
 * @brief 查找DDS主题下的特定回调函数的节点
 * @param topic 指向DDS主题的指针
 * @param callback 要查找的回调函数
 * @return 找到的节点指针，如果没有找到返回NULL
 */
dds_node_t *dds_topic_find(dds_topic_t *topic, dds_callback_t callback);

/**
 * @brief 发布DDS主题，通知所有订阅者
 * @param device 设备指针
 * @param topic 指向DDS主题的指针
 * @param arg 设备参数
 */
void dds_publish(void *device, dds_topic_t *topic, void *arg);

/**
 * @brief 跳过DDS主题的N次订阅
 *
 * @param node 要跳过的节点
 * @param cnt 要跳过的次数
 */
void dds_skip(dds_node_t *node, uint32_t cnt);

/**
 * @brief DDS论调
 *
 */
void dds_poll(void);

/**
 * @brief 创建DDS调度任务
 *
 * @param fn_entry 任务函数
 * @param userdata 用户数据
 * @param priority 优先级
 * @param period_us 周期
 */
void dds_idle_create(dds_task_fn_t fn_entry, void *userdata, uint16_t priority, uint32_t period_us);

/**
 * @brief 删除DDS调度任务
 *
 * @param fn_entry 任务函数
 */
void dds_idle_delete(dds_task_fn_t fn_entry);

/**
 * @brief 初始化DDS
 *
 * @param fn_entry 入口函数
 * @param userdata 用户数据
 * @param priority 优先级
 */
void dds_init_create(dds_task_fn_t fn_init, void *userdata, uint16_t priority);

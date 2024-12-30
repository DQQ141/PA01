/**
 * @file list.h
 * @author WittXie
 * @brief 链表，带互斥锁、动态内存接口、数据版本管理 防ABA
 * @version 0.1
 * @date 2024-08-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

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
 * @brief 链表节点结构体
 */
typedef struct list_node_t
{
    struct list_node_t *prev; /**< 前一个节点 */
    struct list_node_t *next; /**< 下一个节点 */
    void *data;               /**< 节点数据 */
    uint32_t create_version;  /**< 出生号 */
} list_node_t;

/**
 * @brief 链表结构体
 */
typedef struct list_t
{
    list_node_t *head; /**< 链表头节点 */
    list_node_t *tail; /**< 链表尾节点 */
    int length;        /**< 链表长度 */
    void *mutex;       /**< 互斥锁 */
    uint32_t version;  /**< 版本号，用于避免ABA问题 */
} list_t;

/**
 * @brief 遍历宏定义
 *
 * @param _p_list 链表指针
 * @param _do_something 遍历操作
 *
 * @note LIST_FULL_TRAVERSE 遍历操作中不能对链表进行增删操作，否则会导致链表遍历异常
 * @note LIST_TRAVERSE 遍历操作中可以对链表进行增删操作，但可能会遍历不全
 */
#define LIST_TRAVERSE_NODE (_node)
#define LIST_TRAVERSE(_p_list, _do_something)                \
    {                                                        \
        ASSERT((_p_list) != NULL);                           \
        list_node_t *_node = (_p_list)->head;                \
        list_node_t *_temp;                                  \
        (_p_list)->version++;                                \
        for (uint32_t _i = 0; _i < (_p_list)->length; _i++)  \
        {                                                    \
            if (_node == NULL)                               \
            {                                                \
                break;                                       \
            }                                                \
            if (!MUTEX_LOCK(&(_p_list)->mutex))              \
            {                                                \
                break;                                       \
            }                                                \
            _temp = _node->next;                             \
            MUTEX_UNLOCK(&(_p_list)->mutex);                 \
            if (_node->create_version != (_p_list)->version) \
            {                                                \
                _do_something;                               \
            }                                                \
            _node = _temp;                                   \
        }                                                    \
    }

#define LIST_FULL_TRAVERSE(_p_list, _do_something) \
    {                                              \
        ASSERT((_p_list) != NULL);                 \
        list_node_t *_node = (_p_list)->head;      \
        list_node_t *_temp;                        \
        (_p_list)->version++;                      \
        while (_node)                              \
        {                                          \
            if (_node == NULL)                     \
            {                                      \
                break;                             \
            }                                      \
            if (!MUTEX_LOCK(&(_p_list)->mutex))    \
            {                                      \
                break;                             \
            }                                      \
            _temp = _node->next;                   \
            MUTEX_UNLOCK(&(_p_list)->mutex);       \
            _do_something;                         \
            _node = _temp;                         \
        }                                          \
    }

#define list_length_get(_p_list) ((_p_list)->length)
#define list_is_empty(_p_list) ((list_length_get(_p_list) == 0) ? true : false)

/**
 * @brief 初始化链表
 *
 * @param list 链表地址
 */
void list_init(list_t *list);

/**
 * @brief 销毁链表
 *
 * @param list 链表地址
 */
void list_deinit(list_t *list);

/**
 * @brief 创建节点
 *
 * @param data 数据地址
 * @return list_node_t* 新节点地址
 */
list_node_t *list_node_create(void *data);

/**
 * @brief 销毁节点
 *
 * @param list 链表指针
 * @param node 节点指针
 */
void list_destroy(list_t *list, list_node_t *node);

/**
 * @brief 销毁所有节点
 *
 * @param list 链表指针
 */
void list_destroy_all(list_t *list);

/**
 * @brief 插入到指定节点后面
 *
 * @param list 链表指针
 * @param node 指定节点指针
 * @param new_node 新节点指针
 */
void list_insert_after(list_t *list, list_node_t *node, list_node_t *new_node);

/**
 * @brief 插入到指定节点前面
 *
 * @param list 链表指针
 * @param node 指定节点指针
 * @param new_node 新节点指针
 */
void list_insert_before(list_t *list, list_node_t *node, list_node_t *new_node);

/**
 * @brief 插入到表头
 *
 * @param list 链表指针
 * @param new_node 新节点指针
 */
void list_insert_head(list_t *list, list_node_t *new_node);

/**
 * @brief 插入到表尾
 *
 * @param list 链表指针
 * @param new_node 新节点指针
 */
void list_insert_tail(list_t *list, list_node_t *new_node);

/**
 * @brief 获取表尾节点
 *
 * @param list 链表指针
 * @return list_node_t* 表尾节点指针
 */
list_node_t *list_get_tail(list_t *list);

/**
 * @brief 获取表头节点
 *
 * @param list 链表指针
 * @return list_node_t* 表头节点指针
 */
list_node_t *list_get_head(list_t *list);

/**
 * @brief 按数据查找节点
 *
 * @param list 链表指针
 * @param data 数据指针
 * @return list_node_t* 匹配的节点指针
 */
list_node_t *list_find_node(list_t *list, void *data);

/**
 * @brief 按传递的函数方法查找节点
 *
 * @param list 链表指针
 * @param match 匹配函数指针
 * @return list_node_t* 匹配的节点指针; 返回值 1：找到； 返回值 0：未找到
 */
list_node_t *list_find_match_node(list_t *list, int8_t (*match)(list_node_t *));

/**
 * @brief 按传递的函数方法排序
 *
 * @param list 链表指针
 * @param compare 比较函数指针；返回值 正序：返回>=0，逆序：返回<0
 */
void list_sort(list_t *list, int8_t (*compare)(list_node_t *, list_node_t *));

/**
 * @brief 遍历链表
 *
 * @param list 链表指针
 * @param callback 回调函数指针
 */
void list_traverse(list_t *list, void (*callback)(list_node_t *));

/**
 * @brief 规则比较插入
 *
 * @param list 链表指针
 * @param compare 比较函数指针; 返回值 正序：返回>=0，逆序：返回<0
 * @param new_node 新节点指针
 */
void list_compare_insert(list_t *list, int8_t (*compare)(list_node_t *node, list_node_t *new_node), list_node_t *new_node);

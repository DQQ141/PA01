/**
 * @file sort.h
 * @author WittXie
 * @brief 快速排序
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

typedef enum
{
    SORT_UINT8 = 0,
    SORT_UINT16,
    SORT_UINT32,
    SORT_UINT64,
    SORT_INT8,
    SORT_INT16,
    SORT_INT32,
    SORT_INT64,
    SORT_FLOAT,
    SORT_DOUBLE,
} sort_data_type;

// 快速排序函数
void sort(sort_data_type sdt, void *array, uint32_t length);

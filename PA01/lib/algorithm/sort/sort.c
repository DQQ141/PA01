/**
 * @file sort.c
 * @author WittXie
 * @brief 堆排序（Heap Sort）实现
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 */

#include "sort.h"
#include <stdlib.h>
#include <string.h>

// 数据类型对应的比较函数
static int compare_uint8(const void *a, const void *b)
{
    return *(uint8_t *)a - *(uint8_t *)b;
}

static int compare_int8(const void *a, const void *b)
{
    return *(int8_t *)a - *(int8_t *)b;
}

static int compare_uint16(const void *a, const void *b)
{
    return *(uint16_t *)a - *(uint16_t *)b;
}

static int compare_int16(const void *a, const void *b)
{
    return *(int16_t *)a - *(int16_t *)b;
}

static int compare_uint32(const void *a, const void *b)
{
    return (*(uint32_t *)a > *(uint32_t *)b) - (*(uint32_t *)a < *(uint32_t *)b);
}

static int compare_int32(const void *a, const void *b)
{
    return (*(int32_t *)a > *(int32_t *)b) - (*(int32_t *)a < *(int32_t *)b);
}

static int compare_uint64(const void *a, const void *b)
{
    return (*(uint64_t *)a > *(uint64_t *)b) - (*(uint64_t *)a < *(uint64_t *)b);
}

static int compare_int64(const void *a, const void *b)
{
    return (*(int64_t *)a > *(int64_t *)b) - (*(int64_t *)a < *(int64_t *)b);
}

static int compare_float(const void *a, const void *b)
{
    return (*(float *)a > *(float *)b) - (*(float *)a < *(float *)b);
}

static int compare_double(const void *a, const void *b)
{
    return (*(double *)a > *(double *)b) - (*(double *)a < *(double *)b);
}

// 堆排序主函数
void sort(sort_data_type sdt, void *array, uint32_t length)
{
    ASSERT(length >= 2, "length = %u", length);

    switch (sdt)
    {
    case SORT_UINT8:
        qsort(array, length, sizeof(uint8_t), compare_uint8); // 使用 qsort 对数组进行排序
        break;
    case SORT_UINT16:
        qsort(array, length, sizeof(uint16_t), compare_uint16); // 使用 qsort 对数组进行排序
        break;
    case SORT_UINT32:
        qsort(array, length, sizeof(uint32_t), compare_uint32); // 使用 qsort 对数组进行排序
        break;
    case SORT_UINT64:
        qsort(array, length, sizeof(uint64_t), compare_uint64); // 使用 qsort 对数组进行排序
        break;
    case SORT_INT8:
        qsort(array, length, sizeof(int8_t), compare_int8); // 使用 qsort 对数组进行排序
        break;
    case SORT_INT16:
        qsort(array, length, sizeof(int16_t), compare_int16); // 使用 qsort 对数组进行排序
        break;
    case SORT_INT32:
        qsort(array, length, sizeof(int32_t), compare_int32); // 使用 qsort 对数组进行排序
        break;
    case SORT_INT64:
        qsort(array, length, sizeof(int64_t), compare_int64); // 使用 qsort 对数组进行排序
        break;
    case SORT_FLOAT:
        qsort(array, length, sizeof(float), compare_float); // 使用 qsort 对数组进行排序
        break;
    case SORT_DOUBLE:
        qsort(array, length, sizeof(double), compare_double); // 使用 qsort 对数组进行排序
        break;
    default:
        ASSERT(false);
        return;
    }
}

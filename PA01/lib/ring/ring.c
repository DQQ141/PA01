#include "./ring.h"

// 初始化环形队列
bool ring_init(ring_t *ring, uint32_t buff_size)
{
    ASSERT(ring != NULL);
    ASSERT(buff_size > 0);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return false;
    }
    ring->head = 0;
    ring->tail = 0;
    ring->size = buff_size;
    ring->version = 0;
    ring->buff = MALLOC(buff_size);

    if (ring->buff == NULL)
    {
        return false;
    }
    memset(ring->buff, 0, buff_size);
    MUTEX_UNLOCK(&ring->mutex); // 加锁
    return true;
}

// 销毁环形队列
void ring_deinit(ring_t *ring)
{
    ASSERT(ring != NULL);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    // 清除信息
    ring->head = 0;
    ring->tail = 0;
    ring->size = 0;
    ring->version = 0;

    FREE(ring->buff);
    ring->buff = NULL;
    MUTEX_UNLOCK(&ring->mutex); // 加锁
}

uint32_t ring_enqueue(ring_t *ring, const uint8_t *data, uint32_t length)
{
    ASSERT(ring != NULL);
    ASSERT(data != NULL);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return 0;
    }

    if (length == 0)
    {
        MUTEX_UNLOCK(&ring->mutex);
        return 0;
    }

    uint32_t space = ring_remain_size(ring);
    if (length > space)
    {
        length = space;
        ERROR("ring is full, length: %d, space: %d", length, space);
    }

    if (length > 0)
    {
        uint32_t part1 = ring->size - ring->tail; // 第一部分长度
        if (length < part1)
        {
            part1 = length; // 如果第一部分就能放下所有数据，则不需要第二部分
        }

        memcpy(ring->buff + ring->tail, data, part1); // 复制第一部分

        if (length > part1)
        {                                                     // 如果还有剩余数据需要复制
            memcpy(ring->buff, data + part1, length - part1); // 复制第二部分
            ring->tail = length - part1;                      // 更新尾指针位置
        }
        else
        {
            ring->tail += part1; // 更新尾指针位置
            if (ring->tail == ring->size)
            { // 如果到达末尾则回到开始处
                ring->tail = 0;
            }
        }
    }
    else
    {
        length = 0;
    }

    ring->version++;
    MUTEX_UNLOCK(&ring->mutex);

    return length; // 返回实际写入的长度
}
uint32_t ring_dequeue(ring_t *ring, uint8_t *data, uint32_t length)
{
    ASSERT(ring != NULL);
    ASSERT(data != NULL);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return 0;
    }

    if (length == 0)
    {
        MUTEX_UNLOCK(&ring->mutex);
        return 0;
    }

    uint32_t data_size = ring_data_size(ring);
    if (length > data_size)
    {
        length = data_size;
    }

    if (length > 0)
    {
        uint32_t part1 = ring->size - ring->head; // 第一部分长度
        if (length < part1)
        {
            part1 = length; // 如果第一部分就能放下所有数据，则不需要第二部分
        }

        memcpy(data, ring->buff + ring->head, part1); // 复制第一部分

        if (length > part1)
        {                                                     // 如果还有剩余数据需要复制
            memcpy(data + part1, ring->buff, length - part1); // 复制第二部分
            ring->head = length - part1;                      // 更新头指针位置
        }
        else
        {
            ring->head += part1; // 更新头指针位置
            if (ring->head == ring->size)
            { // 如果到达末尾则回到开始处
                ring->head = 0;
            }
        }
    }
    else
    {
        length = 0;
    }

    ring->version++;
    MUTEX_UNLOCK(&ring->mutex);

    return length; // 返回实际读取的长度
}

// 丢弃数据
uint32_t ring_discard(ring_t *ring, uint32_t length)
{
    ASSERT(ring != NULL);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return 0;
    }

    if (length == 0)
    {
        MUTEX_UNLOCK(&ring->mutex);
        return 0;
    }

    uint32_t data_size = ring_data_size(ring);
    if (length > data_size)
    {
        length = data_size;
    }

    if (length > 0)
    {
        uint32_t part1 = ring->size - ring->head; // 第一部分长度
        if (length < part1)
        {
            part1 = length; // 如果第一部分就能放下所有数据，则不需要第二部分
        }

        if (length > part1)
        {                                // 如果还有剩余数据需要复制
            ring->head = length - part1; // 更新头指针位置
        }
        else
        {
            ring->head += part1; // 更新头指针位置
            if (ring->head == ring->size)
            { // 如果到达末尾则回到开始处
                ring->head = 0;
            }
        }
    }
    else
    {
        length = 0;
    }

    ring->version++;
    MUTEX_UNLOCK(&ring->mutex);

    return length; // 返回实际读取的长度
}

// 获取环形队列的可用空间
uint32_t ring_remain_size(ring_t *ring)
{
    ASSERT(ring != NULL);

    if (ring_is_full(ring))
    {
        return 0;
    }

    if (ring->tail >= ring->head)
    {
        return ring->size - (ring->tail - ring->head);
    }
    else
    {
        return ring->head - ring->tail;
    }
}

// 获取环形队列的数据量
uint32_t ring_data_size(ring_t *ring)
{
    ASSERT(ring != NULL);

    if (ring_is_full(ring))
    {
        return ring->size - 1;
    }

    if (ring->tail >= ring->head)
    {
        return ring->tail - ring->head;
    }
    else
    {
        return ring->size - (ring->head - ring->tail);
    }
}

// 重置环形队列
void ring_clear(ring_t *ring)
{
    ASSERT(ring != NULL);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    ring->head = 0;
    ring->tail = 0;
    ring->version = 0;

    MUTEX_UNLOCK(&ring->mutex);
}

uint32_t ring_peek(ring_t *ring, uint8_t *data, uint32_t length)
{
    ASSERT(ring != NULL);
    ASSERT(data != NULL);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return 0;
    }

    uint32_t data_size = ring_data_size(ring);
    if (length > data_size)
    {
        length = data_size;
    }

    if (length > 0)
    {
        uint32_t head = ring->head;

        if (head + length > ring->size)
        {
            uint32_t part1 = ring->size - head;
            memcpy(data, ring->buff + head, part1); // 第一部分

            uint32_t part2 = length - part1;         // 第二部分的长度
            memcpy(data + part1, ring->buff, part2); // 第二部分
        }
        else
        {
            memcpy(data, ring->buff + head, length);
        }
    }
    else
    {
        length = 0;
    }

    MUTEX_UNLOCK(&ring->mutex); // 解锁

    return length; // 返回实际窥视的长度
}
// 删除环形队列指定位置的数据
uint32_t ring_delete(ring_t *ring, uint32_t index, uint32_t data_length)
{
    ASSERT(ring != NULL);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return 0;
    }

    uint32_t data_size = ring_data_size(ring);
    if (index >= data_size)
    {
        MUTEX_UNLOCK(&ring->mutex);
        return 0;
    }

    if (data_length > data_size - index)
    {
        data_length = data_size - index;
    }

    if (data_length > 0)
    {
        uint32_t head = ring->head + index;

        if (head >= ring->size)
        {
            head -= ring->size;
        }

        uint32_t tail = head + data_length;

        if (tail > ring->size)
        {
            tail -= ring->size;
        }

        // 计算删除后尾指针的新位置
        uint32_t new_tail_position = ((ring->tail - data_length) % ring->size + ring->size) % ring->size;

        // 移动内存以覆盖删除的部分
        if (tail <= head) // 需要绕回的情况
        {
            uint32_t wrap_around_part = ring->size - head; // 从缓冲区末尾到开始的数据
            uint32_t remaining_part = new_tail_position;   // 从缓冲区开始的数据

            memmove(ring->buff + head, ring->buff + tail, wrap_around_part);
            if (remaining_part > 0)
            {
                memcpy(ring->buff, ring->buff + ring->size, remaining_part);
            }
        }
        else // 不需要绕回
        {
            memmove(ring->buff + head, ring->buff + tail, new_tail_position - tail);
        }

        ring->tail = new_tail_position; // 正确更新尾指针
    }

    ring->version++;
    MUTEX_UNLOCK(&ring->mutex); // 解锁

    return data_length; // 返回实际删除的字节数
}

// 插入数据到环形队列指定位置
uint32_t ring_insert(ring_t *ring, uint32_t index, const uint8_t *data, uint32_t data_length)
{
    ASSERT(ring != NULL);
    ASSERT(data != NULL);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        // ERROR("mutex lock failed");
        return 0;
    }

    // 检查索引是否越界
    if (index >= ring->size)
    {
        // ERROR("Index out of range");
        MUTEX_UNLOCK(&ring->mutex);
        return 0;
    }

    uint32_t space = ring_remain_size(ring);
    if (data_length > space)
    {
        data_length = space;
    }

    if (data_length > 0)
    {
        uint32_t head = (ring->head + index) % ring->size; // 使用取模运算

        // 分两段复制数据
        uint32_t first_part_len = (data_length < (ring->size - head)) ? data_length : (ring->size - head);
        memcpy(ring->buff + head, data, first_part_len);

        if (first_part_len < data_length)
        {
            memcpy(ring->buff, data + first_part_len, data_length - first_part_len);
        }

        // 更新 tail 指针
        ring->tail = (ring->tail + data_length) % ring->size;

        // 更新版本号
        ring->version++;
    }

    MUTEX_UNLOCK(&ring->mutex); // 解锁

    return data_length; // 返回实际插入的字节数
}

// 从环形队列指定位置提出数据
uint32_t ring_takeout(ring_t *ring, uint32_t index, uint8_t *data, uint32_t length)
{
    ASSERT(ring != NULL);
    ASSERT(data != NULL);

    if (!MUTEX_LOCK(&ring->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return 0;
    }

    uint32_t data_size = ring_data_size(ring);
    if (index >= data_size)
    {
        MUTEX_UNLOCK(&ring->mutex);
        return 0;
    }

    if (length > data_size - index)
    {
        length = data_size - index;
    }

    if (length > 0)
    {
        uint32_t head = (ring->head + index) % ring->size;
        uint32_t tail = (head + length) % ring->size;

        if (tail > head)
        {
            memcpy(data, ring->buff + head, length);
            memmove(ring->buff + head, ring->buff + tail, ring->size - tail);
            ring->tail = (ring->tail + length) % ring->size;
        }
        else
        {
            memcpy(data, ring->buff + head, length);
            memmove(ring->buff + head, ring->buff + tail, ring->size - tail);
            ring->tail = (ring->tail + length) % ring->size;
        }
    }

    ring->version++;
    MUTEX_UNLOCK(&ring->mutex);

    return length; // 返回实际提出的字节数
}

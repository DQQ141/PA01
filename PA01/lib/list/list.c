#include "./list.h"
#include <stdio.h>

// 初始化链表
void list_init(list_t *list)
{
    ASSERT(list != NULL); // 断言list不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    list->head = NULL; // 设置头节点为空
    list->tail = NULL; // 设置尾节点为空
    list->length = 0;  // 长度
    list->version = 0; // 设置版本号为0

    MUTEX_UNLOCK(&list->mutex);
}

// 创建新节点
list_node_t *list_node_create(void *data)
{
    list_node_t *new_node = (list_node_t *)MALLOC(sizeof(list_node_t));
    ASSERT(new_node != NULL); // 断言new_node不为空
    memset(new_node, 0, sizeof(list_node_t));

    new_node->prev = NULL; // 初始化前驱节点为空
    new_node->next = NULL; // 初始化后继节点为空
    new_node->data = data; // 设置节点数据

    return new_node; // 返回新创建的节点
}

// 删除节点
static void slist_remove(list_t *list, list_node_t *node)
{
    ASSERT(list != NULL); // 断言list不为空
    ASSERT(node != NULL); // 断言node不为空
    ASSERT(!(list->head == NULL || list->tail == NULL));

    if (node->prev != NULL)
    {
        node->prev->next = node->next; // 将前一个节点的next指向当前节点的next
    }
    else
    {
        list->head = node->next; // 如果当前节点是头节点，更新头节点
    }
    if (node->next != NULL)
    {
        node->next->prev = node->prev; // 将下一个节点的prev指向当前节点的prev
    }
    else
    {
        list->tail = node->prev; // 如果当前节点是尾节点，更新尾节点
    }
    list->length--; // 减少长度
}

void list_remove(list_t *list, list_node_t *node)
{
    ASSERT(list != NULL); // 断言list不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    slist_remove(list, node); // 删除节点

    MUTEX_UNLOCK(&list->mutex); // 解锁
}

void list_remove_all(list_t *list)
{
    ASSERT(list != NULL); // 断言list不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    list_node_t *node = list->head; // 获取链表头节点
    while (node != NULL)
    {
        list_node_t *next = node->next; // 保存下一个节点
        slist_remove(list, node);       // 销毁节点
        node = next;                    // 移动到下一个节点
    }
    list->head = NULL; // 设置头节点为空
    list->tail = NULL; // 设置尾节点为空
    list->length = 0;  // 长度

    MUTEX_UNLOCK(&list->mutex); // 解锁
}

void list_destroy(list_t *list, list_node_t *node)
{
    ASSERT(list != NULL); // 断言list不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    slist_remove(list, node); // 销毁节点
    FREE(node);               // 释放节点内存

    MUTEX_UNLOCK(&list->mutex); // 解锁
}

void list_destroy_all(list_t *list)
{
    ASSERT(list != NULL); // 断言list不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    list_node_t *node = list->head; // 获取链表头节点
    while (node != NULL)
    {
        list_node_t *next = node->next; // 保存下一个节点
        slist_remove(list, node);       // 销毁节点
        FREE(node);                     // 释放节点内存
        node = next;                    // 移动到下一个节点
    }
    list->head = NULL; // 设置头节点为空
    list->tail = NULL; // 设置尾节点为空
    list->length = 0;  // 长度

    MUTEX_UNLOCK(&list->mutex); // 解锁
}

// 销毁链表
void list_deinit(list_t *list)
{
    ASSERT(list != NULL); // 断言list不为空

    list_destroy_all(list);
    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }
    // 清除信息
    list->head = NULL; // 设置头节点为空
    list->tail = NULL; // 设置尾节点为空
    list->length = 0;  // 长度
    list->version = 0; // 设置版本号为0

    MUTEX_UNLOCK(&list->mutex);
    list->mutex = NULL; // 设置互斥锁为空
}

// 插入到指定节点后面
void list_insert_after(list_t *list, list_node_t *node, list_node_t *new_node)
{
    ASSERT(list != NULL);     // 断言list不为空
    ASSERT(node != NULL);     // 断言node不为空
    ASSERT(new_node != NULL); // 断言new_node不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    new_node->prev = node;       // 设置新节点的前驱为当前节点
    new_node->next = node->next; // 设置新节点的后继为当前节点的后继
    if (node->next != NULL)
    {
        node->next->prev = new_node; // 如果当前节点的后继不为空，更新其后继的前驱
    }
    else
    {
        list->tail = new_node; // 如果当前节点是尾节点，更新尾节点
    }
    node->next = new_node;                    // 更新当前节点的后继为新节点
    list->length++;                           // 长度增加
    new_node->create_version = list->version; // 设置节点的版本号
    MUTEX_UNLOCK(&list->mutex);               // 解锁
}

// 插入到指定节点前面
void list_insert_before(list_t *list, list_node_t *node, list_node_t *new_node)
{
    ASSERT(list != NULL);     // 断言list不为空
    ASSERT(node != NULL);     // 断言node不为空
    ASSERT(new_node != NULL); // 断言new_node不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    new_node->prev = node->prev; // 设置新节点的前驱为当前节点的前驱
    new_node->next = node;       // 设置新节点的后继为当前节点
    if (node->prev != NULL)
    {
        node->prev->next = new_node; // 如果当前节点的前驱不为空，更新其前驱的后继
    }
    else
    {
        list->head = new_node; // 如果当前节点是头节点，更新头节点
    }
    node->prev = new_node;                    // 更新当前节点的前驱为新节点
    list->length++;                           // 长度增加
    new_node->create_version = list->version; // 设置节点的版本号
    MUTEX_UNLOCK(&list->mutex);               // 解锁
}

// 插入到表头
void list_insert_head(list_t *list, list_node_t *new_node)
{
    ASSERT(list != NULL);     // 断言list不为空
    ASSERT(new_node != NULL); // 断言new_node不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    new_node->prev = NULL;       // 设置新节点的前驱为空
    new_node->next = list->head; // 设置新节点的后继为当前头节点
    if (list->head != NULL)
    {
        list->head->prev = new_node; // 如果头节点不为空，更新其前驱为新节点
    }
    else
    {
        list->tail = new_node; // 如果头节点为空，更新尾节点为新节点
    }
    list->head = new_node;                    // 更新头节点为新节点
    list->length++;                           // 长度增加
    new_node->create_version = list->version; // 设置节点的版本号
    MUTEX_UNLOCK(&list->mutex);               // 解锁
}

// 插入到表尾
void list_insert_tail(list_t *list, list_node_t *new_node)
{
    ASSERT(list != NULL);     // 断言list不为空
    ASSERT(new_node != NULL); // 断言new_node不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    new_node->prev = list->tail; // 设置新节点的前驱为当前尾节点
    new_node->next = NULL;       // 设置新节点的后继为空
    if (list->tail != NULL)
    {
        list->tail->next = new_node; // 如果尾节点不为空，更新其后继为新节点
    }
    else
    {
        list->head = new_node; // 如果尾节点为空，更新头节点为新节点
    }
    list->tail = new_node;                    // 更新尾节点为新节点
    list->length++;                           // 长度增加
    new_node->create_version = list->version; // 设置节点的版本号
    MUTEX_UNLOCK(&list->mutex);               // 解锁
}

// 获取表尾节点
list_node_t *list_get_tail(list_t *list)
{
    ASSERT(list != NULL); // 断言list不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return NULL;
    }

    list_node_t *tail = list->tail; // 获取尾节点
    MUTEX_UNLOCK(&list->mutex);     // 解锁
    return tail;                    // 返回尾节点
}

// 获取表头节点
list_node_t *list_get_head(list_t *list)
{
    ASSERT(list != NULL); // 断言list不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return NULL;
    }

    list_node_t *head = list->head; // 获取头节点
    MUTEX_UNLOCK(&list->mutex);     // 解锁
    return head;                    // 返回头节点
}

// 按传递的函数方法查找节点
list_node_t *list_find_node(list_t *list, void *data)
{
    ASSERT(list != NULL); // 断言list不为空
    ASSERT(data != NULL); // 断言match不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return NULL;
    }

    list_node_t *node = list->head; // 获取头节点
    while (node != NULL)
    {
        if (data == node->data) // 如果匹配函数返回真
        {
            MUTEX_UNLOCK(&list->mutex); // 解锁
            return node;                // 返回当前节点
        }
        node = node->next; // 移动到下一个节点
    }
    MUTEX_UNLOCK(&list->mutex); // 解锁
    return NULL;                // 如果没有找到匹配的节点，返回空
}

// 按传递的函数方法查找节点
list_node_t *list_find_match_node(list_t *list, int8_t (*match)(list_node_t *))
{
    ASSERT(list != NULL);  // 断言list不为空
    ASSERT(match != NULL); // 断言match不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return NULL;
    }

    list_node_t *node = list->head; // 获取头节点
    while (node != NULL)
    {
        if (match(node)) // 如果匹配函数返回真
        {
            MUTEX_UNLOCK(&list->mutex); // 解锁
            return node;                // 返回当前节点
        }
        node = node->next; // 移动到下一个节点
    }
    MUTEX_UNLOCK(&list->mutex); // 解锁
    return NULL;                // 如果没有找到匹配的节点，返回空
}

// 按传递的函数方法排序
void list_sort(list_t *list, int8_t (*compare)(list_node_t *, list_node_t *))
{
    ASSERT(list != NULL);    // 断言list不为空
    ASSERT(compare != NULL); // 断言compare不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    // 使用冒泡排序
    bool swapped;
    list_node_t *ptr1;
    list_node_t *lptr = NULL;

    if (list->head == NULL) // 如果链表为空
    {
        MUTEX_UNLOCK(&list->mutex); // 解锁
        return;                     // 直接返回
    }

    do
    {
        swapped = false;   // 初始化交换标志为假
        ptr1 = list->head; // 获取头节点

        while (ptr1->next != lptr) // 遍历链表
        {
            if (compare(ptr1, ptr1->next) > 0) // 如果当前节点大于下一个节点
            {
                // 交换节点
                list_node_t *temp = ptr1->next;
                if (ptr1->prev != NULL)
                {
                    ptr1->prev->next = temp; // 更新前一个节点的后继
                }
                else
                {
                    list->head = temp; // 更新头节点
                }
                if (temp->next != NULL)
                {
                    temp->next->prev = ptr1; // 更新后一个节点的前驱
                }
                else
                {
                    list->tail = ptr1; // 更新尾节点
                }
                ptr1->next = temp->next; // 更新当前节点的后继
                temp->prev = ptr1->prev; // 更新临时节点的前驱
                ptr1->prev = temp;       // 更新当前节点的前驱
                temp->next = ptr1;       // 更新临时节点的后继
                swapped = true;          // 设置交换标志为真
            }
            else
            {
                ptr1 = ptr1->next; // 移动到下一个节点
            }
        }
        lptr = ptr1; // 更新lptr
    } while (swapped); // 如果发生交换，继续循环

    MUTEX_UNLOCK(&list->mutex); // 解锁
}

// 匹配插入
void list_compare_insert(list_t *list, int8_t (*compare)(list_node_t *node, list_node_t *new_node), list_node_t *new_node)
{
    ASSERT(list != NULL);     // 断言list不为空
    ASSERT(compare != NULL);  // 断言compare不为空
    ASSERT(new_node != NULL); // 断言new_node不为空

    if (!MUTEX_LOCK(&list->mutex)) // 加锁
    {
        ERROR("mutex lock failed");
        return;
    }

    list_node_t *node = list->head; // 获取头节点
    list_node_t *prev = NULL;       // 保存前一个节点

    while (node != NULL && compare(node, new_node) < 0)
    {
        prev = node;
        node = node->next; // 移动到下一个节点
    }

    if (prev == NULL) // 插入到表头
    {
        new_node->next = list->head;
        new_node->prev = NULL;
        if (list->head != NULL)
        {
            list->head->prev = new_node;
        }
        list->head = new_node;
        if (list->tail == NULL)
        {
            list->tail = new_node;
        }
    }
    else // 插入到prev节点后面
    {
        new_node->next = prev->next;
        new_node->prev = prev;
        prev->next = new_node;
        if (new_node->next != NULL)
        {
            new_node->next->prev = new_node;
        }
        else
        {
            list->tail = new_node;
        }
    }

    list->length++;             // 长度增加
    MUTEX_UNLOCK(&list->mutex); // 解锁
}

// 遍历链表并调用回调函数
void list_traverse(list_t *list, void (*callback)(list_node_t *))
{
    ASSERT(list != NULL);     // 断言list不为空
    ASSERT(callback != NULL); // 断言callback不为空

    list_node_t *node = list->head;
    list_node_t *temp;
    list->version++; // 增加版本号
    for (uint32_t i = 0; i < list->length; i++)
    {
        if (node == NULL)
        {
            return;
        }
        if (!MUTEX_LOCK(&list->mutex)) // 加锁
        {
            ERROR("mutex lock failed");
            return;
        }
        node = temp;
        MUTEX_UNLOCK(&list->mutex); // 解锁
        if (node->create_version != (list)->version)
        {
            callback(node);
        }
        temp = node->next;
    }
}

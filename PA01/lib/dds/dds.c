#include "./dds.h"

// 用节点指针订阅
dds_node_t *dds_subcribe_with_node(dds_topic_t *topic, dds_node_t *dds_node)
{
    ASSERT(topic != NULL);
    ASSERT(dds_node != NULL);

    // 遍历节点, 按优先级插队
    LIST_FULL_TRAVERSE(topic, {
        if (dds_node->priority < ((dds_node_t *)(LIST_TRAVERSE_NODE->data))->priority)
        {
            list_insert_before(topic, LIST_TRAVERSE_NODE, dds_node->list_node);
            return dds_node;
        }
    });
    list_insert_tail(topic, dds_node->list_node);
    return dds_node;
}

// 用回调函数订阅
dds_node_t *dds_subcribe(dds_topic_t *topic, uint16_t priority, dds_callback_t callback, void *userdata)
{
    ASSERT(topic != NULL);
    ASSERT(callback != NULL);

    dds_node_t *dds_node = MALLOC(sizeof(dds_node_t));
    if (dds_node == NULL)
    {
        return NULL;
    }
    memset(dds_node, 0, sizeof(dds_node_t));

    dds_node->list_node = list_node_create(dds_node); // 创建链表节点指向自己
    ASSERT(dds_node->list_node != NULL);

    // 赋值
    dds_node->callback = callback; // 回调函数
    dds_node->priority = priority; // 优先级
    dds_node->skip_cnt = 0;
    dds_node->userdata = userdata;

    if (dds_subcribe_with_node(topic, dds_node) == NULL)
    {
        FREE(dds_node);
        return NULL;
    }
    else
    {
        return dds_node;
    }
}

// 订阅在指定节点之前
dds_node_t *dds_subcribe_before(dds_topic_t *topic, dds_node_t *target_node, dds_callback_t callback, void *userdata)
{
    ASSERT(topic != NULL);
    ASSERT(target_node != NULL);
    ASSERT(callback != NULL);

    dds_node_t *dds_node = MALLOC(sizeof(dds_node_t));
    if (dds_node == NULL)
    {
        return NULL;
    }
    memset(dds_node, 0, sizeof(dds_node_t));

    dds_node->list_node = list_node_create(dds_node); // 创建链表节点指向自己
    ASSERT(dds_node->list_node != NULL);

    // 赋值
    dds_node->callback = callback;              // 回调函数
    dds_node->priority = target_node->priority; // 优先级
    dds_node->userdata = userdata;

    list_insert_before(topic, target_node->list_node, dds_node->list_node);
    return dds_node;
}

// 订阅在指定节点之后
dds_node_t *dds_subcribe_after(dds_topic_t *topic, dds_node_t *target_node, dds_callback_t callback, void *userdata)
{
    ASSERT(topic != NULL);
    ASSERT(target_node != NULL);
    ASSERT(callback != NULL);
    if (topic == NULL || target_node == NULL || callback == NULL)
    {
        return NULL;
    }

    dds_node_t *dds_node = MALLOC(sizeof(dds_node_t));
    if (dds_node == NULL)
    {
        return NULL;
    }
    memset(dds_node, 0, sizeof(dds_node_t));

    dds_node->list_node = list_node_create(dds_node); // 创建链表节点指向自己
    ASSERT(dds_node->list_node != NULL);

    // 赋值
    dds_node->callback = callback;              // 回调函数
    dds_node->priority = target_node->priority; // 优先级
    dds_node->userdata = userdata;

    list_insert_after(topic, target_node->list_node, dds_node->list_node);
    return dds_node;
}

// 用节点指针取消订阅
void dds_unsubcribe_with_node(dds_topic_t *topic, dds_node_t *dds_node)
{
    ASSERT(topic != NULL);
    ASSERT(dds_node != NULL);
    if (topic == NULL || dds_node == NULL)
    {
        return;
    }

    list_destroy(topic, dds_node->list_node);
    FREE(dds_node);
}

// 用回调函数取消订阅
void dds_unsubcribe(dds_topic_t *topic, dds_callback_t callback)
{
    ASSERT(topic != NULL);
    ASSERT(callback != NULL);
    if (topic == NULL || callback == NULL)
    {
        return;
    }

    // 遍历并删除所有符合要求的节点
    LIST_FULL_TRAVERSE(topic, {
        if (((dds_node_t *)(LIST_TRAVERSE_NODE->data))->callback == callback)
        {
            dds_unsubcribe_with_node(topic, (dds_node_t *)(LIST_TRAVERSE_NODE->data));
        }
    });
}

// 取消这个主题的所有订阅
void dds_unsubcribe_all(dds_topic_t *topic)
{
    ASSERT(topic != NULL);
    if (topic == NULL)
    {
        return;
    }

    // 遍历并删除所有节点
    LIST_FULL_TRAVERSE(topic, {
        dds_unsubcribe_with_node(topic, (dds_node_t *)(LIST_TRAVERSE_NODE->data));
    });
}

// 查找一个主题下的回调函数
dds_node_t *dds_topic_find(dds_topic_t *topic, dds_callback_t callback)
{
    ASSERT(topic != NULL);
    ASSERT(callback != NULL);
    if (topic == NULL || callback == NULL)
    {
        return NULL;
    }

    // 遍历所有符合要求的节点
    LIST_FULL_TRAVERSE(topic, {
        if (((dds_node_t *)(LIST_TRAVERSE_NODE->data))->callback == callback)
            return (dds_node_t *)(LIST_TRAVERSE_NODE->data);
    });
    return NULL;
}

// 发布主题
void dds_publish(void *device, dds_topic_t *topic, void *arg)
{
    ASSERT(topic != NULL);
    if (topic == NULL)
    {
        return;
    }

    // 遍历所有符合要求的节点
    LIST_TRAVERSE(topic, {
        if (((dds_node_t *)(LIST_TRAVERSE_NODE->data))->skip_cnt > 0)
        {
            ((dds_node_t *)(LIST_TRAVERSE_NODE->data))->skip_cnt--;
        }
        else
        {
            ((dds_node_t *)(LIST_TRAVERSE_NODE->data))->callback(device, topic, arg, ((dds_node_t *)(LIST_TRAVERSE_NODE->data))->userdata);
        }
    });
}

// dds临时调过一次
void dds_skip(dds_node_t *node, uint32_t cnt)
{
    ASSERT(node != NULL);
    node->skip_cnt = cnt;
}

static dds_topic_t DDS_INIT = {0}; // 初始化主题
static dds_topic_t DDS_IDLE = {0}; // 空闲主题

typedef struct
{
    uint64_t period_us;
    uint64_t last_us;
    void *userdata;
} dds_data_t;

void dds_poll(void)
{
    dds_task_fn_t fn_entry;
    dds_data_t *data;

    // 遍历所有初始化
    LIST_TRAVERSE(&DDS_INIT, {
        fn_entry = (dds_task_fn_t)(((dds_node_t *)(LIST_TRAVERSE_NODE->data))->callback);
        fn_entry(((dds_node_t *)(LIST_TRAVERSE_NODE->data))->userdata);
        dds_unsubcribe(&DDS_INIT, (dds_callback_t)fn_entry);
    });

    // 遍历所有空闲任务
    LIST_TRAVERSE(&DDS_IDLE, {
        fn_entry = (dds_task_fn_t)(((dds_node_t *)(LIST_TRAVERSE_NODE->data))->callback);
        data = ((dds_data_t *)(((dds_node_t *)(LIST_TRAVERSE_NODE->data))->userdata));
        if (is_timeout(data->last_us, data->period_us))
        {
            fn_entry(data->userdata);
            data->last_us = TIMESTAMP_US_GET();
        }
    });
}

// 创建dds调度的任务
void dds_idle_create(dds_task_fn_t fn_entry, void *userdata, uint16_t priority, uint32_t period_us)
{
    ASSERT(fn_entry != NULL);
    if (fn_entry == NULL)
    {
        return;
    }

    dds_data_t *data = MALLOC(sizeof(dds_data_t));
    ASSERT(data != NULL);
    if (data == NULL)
    {
        return;
    }

    data->period_us = period_us;
    data->userdata = userdata;
    data->last_us = 0;
    dds_subcribe(&DDS_IDLE, priority, (dds_callback_t)fn_entry, (void *)data);
}

void dds_idle_delete(dds_task_fn_t fn_entry)
{
    ASSERT(fn_entry != NULL);
    if (fn_entry == NULL)
    {
        return;
    }

    dds_node_t *node = dds_topic_find(&DDS_IDLE, (dds_callback_t)fn_entry);

    // 遍历并删除所有符合要求的节点
    LIST_FULL_TRAVERSE(&DDS_IDLE, {
        if (((dds_node_t *)(LIST_TRAVERSE_NODE->data))->callback == (dds_callback_t)fn_entry)
        {
            dds_unsubcribe_with_node(&DDS_IDLE, (dds_node_t *)(LIST_TRAVERSE_NODE->data));
            FREE(((dds_node_t *)(LIST_TRAVERSE_NODE->data))->userdata);
        }
    });
}

// 创建dds调度的任务
void dds_init_create(dds_task_fn_t fn_init, void *userdata, uint16_t priority)
{
    ASSERT(fn_init != NULL);
    if (fn_init == NULL)
    {
        return;
    }

    dds_subcribe(&DDS_INIT, priority, (dds_callback_t)fn_init, (void *)userdata);
}

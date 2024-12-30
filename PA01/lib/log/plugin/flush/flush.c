/**
 * @file flush.c
 * @author WittXie
 * @brief 缓存插件
 * @version 0.1
 * @date 2024-09-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../log.h"

// 日志推送
static void log_flush_poll(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    log_t *log = (log_t *)device;

    // 从环形缓冲队列中取出日志并发送
    if (ring_data_size(&log->ring_write) > 0)
    {
        uint8_t *buff = NULL;
        buff = (uint8_t *)STACK_MALLOC(log->cfg.buff_size);
        if (buff == NULL)
        {
            return;
        }
        uint32_t count = ring_dequeue(&(log->ring_write), buff, log->cfg.buff_size);
        log->ops.write(buff, count);
        STACK_FREE(buff);
    }
}

// 缓存写入
static void log_flush_write(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    log_t *log = (log_t *)device;
    log_str_t *str = (log_str_t *)arg;

    if (str->length < ring_remain_size(&log->ring_write))
    {
        // 将格式化后的日志信息入队到环形缓冲队列
        ring_enqueue(&log->ring_write, (uint8_t *)str->data, str->length);
    }
    else
    {
        log_dprint(log, "%.*s", str->length, (char *)str->data);
    }
}

// 缓存插件释放
static void log_flush_deinit(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    log_t *log = (log_t *)device;
    if (log == NULL)
    {
        log_print_error(log, "log plugin [log_flush_deinit]: failed, log is null.");
        return;
    }
    ring_deinit(&(log->ring_write));
    log_print_info(log, "log plugin [flush]: disable.");
}

// 缓存初始化
void log_flush_init(log_t *log)
{
    ASSERT(log != NULL);

    if (log->flag.is_inited == false)
    {
        log_print_error(log, "log plugin [log_flush_init]: init failed, log->flag.is_inited = false.");
        return;
    }

    // 从PRINT注销标准输出的订阅
    void log_std_write(void *device, dds_topic_t *topic, void *arg, void *userdata);
    dds_unsubcribe(&log->PRINT, log_std_write);

    // 缓存输出订阅PRINT
    dds_subcribe(&log->PRINT, DDS_PRIORITY_NORMAL, log_flush_write, NULL);
    dds_subcribe(&log->POLL, DDS_PRIORITY_NORMAL, log_flush_poll, NULL);
    dds_subcribe(&log->REFRESH, DDS_PRIORITY_NORMAL, log_flush_poll, NULL);
    dds_subcribe(&log->DEINIT, DDS_PRIORITY_NORMAL, log_flush_deinit, NULL);

    log_print_info(log, "log plugin [flush]: enable.");
    log_flush(log);
}

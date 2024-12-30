/**
 * @file hook.cc
 * @author WittXie
 * @brief 日志打印hook
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./log.h"

// 直接写入
void log_std_write(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    log_t *log = (log_t *)device;

    // 检查TAG
    if (log->cfg.filter != NULL)
    {
        // 检查筛选词
        char *ret = strstr((char *)arg, log->cfg.filter);
        if (ret == NULL)
        {
            // 不存在筛选词，直接返回
            return;
        }
    }

    if (MUTEX_LOCK(&log->mutex)) // 上锁
    {
        log_str_t *str = (log_str_t *)arg;
        log->ops.write((uint8_t *)str->data, str->length); // 写入
        MUTEX_UNLOCK(&log->mutex);                         // 解锁
    }
}

// 打印输出hook
void log_interface_print(log_t *log, const char *format, ...)
{
    if (log == NULL)
    {
        return;
    }

    if (log->flag.is_inited == false ||
        log->ops.write == NULL ||
        log->cfg.level == LOG_LEVEL_NONE)
    {
        return;
    }

    uint32_t length = 0;
    va_list arg;
    uint8_t *buff = NULL;
    buff = (uint8_t *)STACK_MALLOC(log->cfg.buff_size);
    if (buff == NULL)
    {
        return;
    }

    va_start(arg, format);
    length = vsnprintf((char *)(buff), log->cfg.buff_size, format, arg); // 格式化提取内容
    va_end(arg);

    // 发布主题
    log_str_t str = {(char *)buff, length};
    dds_publish(log, &log->PRINT, &str);
    STACK_FREE(buff);
}

// 直接输出
void log_interface_dprint(log_t *log, const char *format, ...)
{
    if (log == NULL)
    {
        return;
    }

    if (log->ops.write == NULL ||
        log->cfg.level == LOG_LEVEL_NONE)
    {
        return;
    }

    log_flush(log);

    uint32_t length = 0;
    va_list arg;

    uint8_t *buff = NULL;
    buff = (uint8_t *)STACK_MALLOC(log->cfg.buff_size);
    if (buff == NULL)
    {
        return;
    }

    va_start(arg, format);
    length = vsnprintf((char *)(buff), log->cfg.buff_size, format, arg); // 格式化提取内容
    va_end(arg);

    // 发布主题
    log_str_t str = {(char *)buff, length};
    log_std_write(log, NULL, &str, NULL);
    STACK_FREE(buff);
}

// log专用DDS
void log_dds_publish(log_t *log, dds_topic_t *topic, const char *format, ...)
{
    if (log == NULL)
    {
        return;
    }

    if (log->flag.is_inited == false ||
        log->ops.write == NULL ||
        log->cfg.level == LOG_LEVEL_NONE)
    {
        return;
    }

    uint32_t length = 0;
    va_list arg;
    uint8_t *buff = NULL;
    buff = (uint8_t *)STACK_MALLOC(log->cfg.buff_size);
    if (buff == NULL)
    {
        return;
    }

    va_start(arg, format);
    length = vsnprintf((char *)(buff), log->cfg.buff_size, format, arg); // 格式化提取内容
    va_end(arg);

    // 发布主题
    log_str_t str = {(char *)buff, length};
    dds_publish(log, topic, &str);
    STACK_FREE(buff);
}

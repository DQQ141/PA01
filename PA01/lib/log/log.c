#include "./log.h"

// 基础
#include "./print.cc"

static void log_print_ico(log_t *log)
{
    ASSERT(log != NULL);

    log_dprint(log, ASCII_CLEAR);
    vdate_t date = build_date_get();
    log_dprint(log, COLOR_H_CYAN "\r                                               \n");
    log_dprint(log, COLOR_H_WHITE "\r                                |\\__/,|   (`\\  \n");
    log_dprint(log, COLOR_H_WHITE "\r                              _.|o o  |_   ) ) \n");
    log_dprint(log, COLOR_H_CYAN "\r┌─────────────────────────────" COLOR_H_WHITE "(((" COLOR_H_CYAN "───" COLOR_H_WHITE "(((" COLOR_H_CYAN "───────┐\n");
    log_dprint(log, COLOR_H_CYAN "\r│     ██╗   ██╗██╗      ██████╗  ██████╗      │\n");
    log_dprint(log, COLOR_H_CYAN "\r│     ██║   ██║██║     ██╔═══██╗██╔════╝      │\n");
    log_dprint(log, COLOR_H_CYAN "\r│     ██║   ██║██║     ██║   ██║██║  ███╗     │\n");
    log_dprint(log, COLOR_H_CYAN "\r│     ╚██╗ ██╔╝██║     ██║   ██║██║   ██║     │\n");
    log_dprint(log, COLOR_H_CYAN "\r│      ╚████╔╝ ███████╗╚██████╔╝╚██████╔╝     │\n");
    log_dprint(log, COLOR_H_CYAN "\r│       ╚═══╝  ╚══════╝ ╚═════╝  ╚═════╝      │\n");
    log_dprint(log, COLOR_H_CYAN "\r│  \\ | /    VLOG with shell plugin            │\n");
    log_dprint(log, COLOR_H_CYAN "\r│ - vlog -  " COLOR_H_WHITE "[ Author: WittXie ]" COLOR_H_CYAN "               │\n");
    log_dprint(log, COLOR_H_CYAN "\r│  / | \\    Build Time: %04d-%02d-%02d %02d:%02d:%02d   │\n", date.year, date.month, date.day, date.hour, date.min, date.sec);
    log_dprint(log, COLOR_H_CYAN "\r└─────────────────────────────────────────────┘\r\n");
    log_dprint(log, COLOR_H_CYAN "\r\n");
}

// 初始化
void log_init(log_t *log)
{
    // 断言
    ASSERT(log != NULL);
    ASSERT(log->cfg.name != NULL);
    ASSERT(log->cfg.buff_size != NULL);
    ASSERT(log->ops.init != NULL);
    ASSERT(log->ops.write != NULL);

    if (log->cfg.buff_size == 0)
    {
        log->flag.is_inited = false;
        log->cfg.level = LOG_LEVEL_NONE;
        return;
    }

    // 检查过滤器
    if (log->cfg.filter != NULL)
    {
        if (log->cfg.filter[0] == '\0')
        {
            log->cfg.filter = NULL;
        }
    }

    // 订阅输出
    dds_subcribe(&log->PRINT, DDS_PRIORITY_NORMAL, log_std_write, NULL);

    // 初始化环形缓冲队列
    bool ret;
    ret = ring_init(&log->ring_read, log->cfg.buff_size);
    if (ret == false)
    {
        log->flag.is_inited = false;
        log_print_error(log, "log_init failed, ring_read create failed.");
        return;
    }

    ret = ring_init(&log->ring_write, log->cfg.buff_size);
    if (ret == false)
    {
        log->flag.is_inited = false;
        log_print_error(log, "log_init failed, ring_write create failed.");
        return;
    }

    // 初始化成功
    log->flag.is_inited = true;

    // 初始化其它部件：发布初始化主题
    dds_publish(log, &log->INIT, NULL);
    log->ops.init();

    // 打印log
    log_print_info(log, "log_init succuss.");
    log_print_ico(log);
}

// 反初始化
void log_deinit(log_t *log)
{
    ASSERT(log != NULL);
    dds_publish(log, &log->DEINIT, NULL); // 发布主题
}

void log_poll(log_t *log)
{
    ASSERT(log != NULL);

    if (log->flag.is_inited == false)
        return;

    log->flag.is_running = true;

    // 读取输入
    uint8_t *buff = NULL;
    buff = STACK_MALLOC(log->cfg.buff_size);
    if (buff != NULL)
    {
        uint32_t length = ring_dequeue(&log->ring_read, buff, log->cfg.buff_size);
        if (length != 0)
        {
            // 发布读取主题
            dds_publish(log, &log->READ, buff);
        }
        STACK_FREE(buff);
    }

    // 发布主题
    dds_publish(log, &log->POLL, NULL);
}

void log_flush(log_t *log)
{
    if (log->flag.is_inited == false)
        return;

    // 发布主题
    dds_publish(log, &log->REFRESH, NULL);
}

/**
 * @file os_monitor.cc
 * @author WittXie
 * @brief 系统状态显示
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./os_bsp.h"

// 打印OS堆使用情况
void os_monitor_heap_usage(void)
{
    print(COLOR_H_GREEN "os heap max: %3.02f%%, %lu/%lu bytes" ASCII_CLEAR_TAIL "\r\nos heap now: %3.02f%%, %lu/%lu bytes" COLOR_L_WHITE ASCII_CLEAR_TAIL "\r\n" ASCII_CLEAR_TAIL,
          (float)(100.0f - (xPortGetMinimumEverFreeHeapSize() * 100.0f / OS_HEAP_SIZE)),
          (uint32_t)(OS_HEAP_SIZE - xPortGetMinimumEverFreeHeapSize()),
          (uint32_t)(OS_HEAP_SIZE),
          (float)(100.0f - (xPortGetFreeHeapSize() * 100.0f / OS_HEAP_SIZE)),
          (uint32_t)(OS_HEAP_SIZE - xPortGetFreeHeapSize()),
          (uint32_t)(OS_HEAP_SIZE));
}

// 任务信息结构体
static struct __s_monitor
{
    int current_page;        // 当前页
    uint32_t task_per_page;  // 每页任务数量
    uint32_t total_pages;    // 总页数
    uint32_t fresh_timegap;  // 时间间隔
    float cpu_usage_real;    // CPU使用率
    float cpu_usage_average; // CPU平均使用率
    bool is_view;            // 是否显示
    bool is_error;           // 是否有错误
} s_monitor = {
    .current_page = 0,
    .task_per_page = 10,
    .total_pages = 1,
    .fresh_timegap = 500,
    .cpu_usage_real = 0.0f,
    .cpu_usage_average = 0.0f,
    .is_view = false,
    .is_error = false,
};

static void task_bubble_sort(TaskStatus_t *tasks, uint32_t num_tasks)
{
    for (uint32_t i = 0; i < num_tasks - 1; ++i)
    {
        for (uint32_t j = 0; j < num_tasks - i - 1; ++j)
        {
            if (tasks[j].xTaskNumber > tasks[j + 1].xTaskNumber)
            {
                TaskStatus_t temp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = temp;
            }
        }
    }
}

static void os_monitor_entry(void *args)
{
    TaskStatus_t *task_status_buff = NULL; // 任务状态
    uint32_t *last_timespent_buff = NULL;  // 任务耗时
    uint64_t *time_alltotal_buff = NULL;   // 任务耗时
    bool is_first = false;
    uint32_t task_size = 0;
    uint32_t idle_task_num = 3;

    uint32_t total_time = 0;
    uint64_t all_total_time = 0;
    uint32_t end_index;   // 结束任务索引
    uint32_t start_index; // 起始任务索引

    for (;;)
    {
        uint32_t new_task_size = uxTaskGetNumberOfTasks(); // 获取任务数量

        if (new_task_size != task_size)
        {
            task_size = new_task_size;

            // 释放旧的内存
            if (task_status_buff != NULL)
            {
                os_free(task_status_buff);
                task_status_buff = NULL;
            }
            if (last_timespent_buff != NULL)
            {
                os_free(last_timespent_buff);
                last_timespent_buff = NULL;
            }
            if (time_alltotal_buff != NULL)
            {
                os_free(time_alltotal_buff);
                time_alltotal_buff = NULL;
            }

            // 分配新的内存
            task_status_buff = (TaskStatus_t *)os_malloc(task_size * sizeof(TaskStatus_t));
            ASSERT(task_status_buff != NULL);

            last_timespent_buff = (uint32_t *)os_malloc(task_size * sizeof(uint32_t));
            ASSERT(last_timespent_buff != NULL);

            time_alltotal_buff = (uint64_t *)os_malloc(task_size * sizeof(uint64_t));
            ASSERT(time_alltotal_buff != NULL);

            memset(task_status_buff, 0, task_size * sizeof(TaskStatus_t));
            memset(last_timespent_buff, 0, task_size * sizeof(uint32_t));
            memset(time_alltotal_buff, 0, task_size * sizeof(uint64_t));
            is_first = false;
            if (s_monitor.is_view)
            {
                print(ASCII_CLEAR);
            }

            // 查找IDLE任务
            uxTaskGetSystemState(task_status_buff, task_size, &total_time);
            for (uint32_t i = 0; i < task_size; i++)
            {
                // 对比任务名字
                if (strcmp(task_status_buff[i].pcTaskName, "IDLE") == 0)
                {
                    idle_task_num = task_status_buff[i].xTaskNumber;
                    break;
                }
            }
        }

        // 计算总页数
        s_monitor.total_pages = (task_size + s_monitor.task_per_page - 1) / s_monitor.task_per_page;

        // 计算当前页
        if (s_monitor.current_page <= 0)
            s_monitor.current_page = 0;
        else if (s_monitor.current_page >= s_monitor.total_pages)
            s_monitor.current_page = s_monitor.total_pages - 1;

        // 计算当前页的起始任务和结束任务索引
        start_index = s_monitor.current_page * s_monitor.task_per_page;
        end_index = start_index + s_monitor.task_per_page;

        // 获取系统状态
        uxTaskGetSystemState(task_status_buff, task_size, &total_time);

        // 排序任务
        task_bubble_sort(task_status_buff, task_size);
        if (end_index > task_size)
        {
            end_index = task_size;
        }

        total_time = 0;
        all_total_time = 0;

        if (is_first == false)
        {
            is_first = true;
            for (uint32_t i = 0; i < task_size; i++)
            {
                last_timespent_buff[i] = task_status_buff[i].ulRunTimeCounter - 1;
            }
        }

        for (uint32_t i = 0; i < task_size; i++)
        {
            total_time += task_status_buff[i].ulRunTimeCounter - last_timespent_buff[i];
            time_alltotal_buff[i] += task_status_buff[i].ulRunTimeCounter - last_timespent_buff[i];
            all_total_time += time_alltotal_buff[i];
        }

        // 打印任务信息
        if (s_monitor.is_view)
        {
            print(ASCII_CURSOR_GOTO("1", "1") COLOR_H_CYAN ASCII_CLEAR_TAIL);
            print("\r\n%-8s %-24s %-9s %-12s %-5s %-6s" ASCII_CLEAR_TAIL,
                  "Num", "Name", "Priority", "Stack(byte)", "Real", "Total");
        }

        for (uint32_t i = start_index; i < end_index; i++)
        {
            float usage_real = ((task_status_buff[i].ulRunTimeCounter - last_timespent_buff[i]) * 100.0f) / total_time;
            float usage_average = ((time_alltotal_buff[i]) * 100.0f) / (all_total_time * 1.0f);

            // 查找IDLE, 计算CPU使用率
            if (task_status_buff[i].xTaskNumber == idle_task_num)
            {
                s_monitor.cpu_usage_real = 100.0f - usage_real;
                s_monitor.cpu_usage_average = 100.0f - usage_average;
            }
            else if (s_monitor.is_error == false)
            {
                if ((unsigned int)task_status_buff[i].usStackHighWaterMark == 0)
                {
                    s_monitor.is_error = true;
                    log_error("Task [%s] stack size is zero.", task_status_buff[i].pcTaskName);
                }
                else if ((unsigned int)task_status_buff[i].usStackHighWaterMark * 4u < 64)
                {
                    s_monitor.is_error = true;
                    log_warn("Task [%s] stack size is too small[%lu].", task_status_buff[i].pcTaskName,
                             (unsigned int)task_status_buff[i].usStackHighWaterMark);
                }
            }

            if (s_monitor.is_view)
            {
                print(COLOR_L_WHITE "\r\n%-8lu %-24s %-9lu %-12lu %-5.02f %-6.02f" ASCII_CLEAR_TAIL,
                      (unsigned int)task_status_buff[i].xTaskNumber,               // 任务序号
                      task_status_buff[i].pcTaskName,                              // 任务名
                      (unsigned int)task_status_buff[i].uxCurrentPriority,         // 任务优先级
                      (unsigned int)task_status_buff[i].usStackHighWaterMark * 4u, // 堆栈剩余大小
                      usage_real,                                                  // 实时CPU使用率
                      usage_average                                                // 总CPU使用率
                );
            }
        }

        if (s_monitor.is_view)
        {
            print("\r\n" ASCII_CLEAR_TAIL);
            print("Page %lu of %lu\r\n", s_monitor.current_page + 1, s_monitor.total_pages);
        }
        // 记录耗时
        for (uint32_t i = 0; i < task_size; i++)
        {
            last_timespent_buff[i] = task_status_buff[i].ulRunTimeCounter;
        }

        if (s_monitor.is_view)
        {
            os_monitor_heap_usage();
            print(ASCII_CLEAR_TAIL COLOR_L_WHITE "\r\n");
            flush();
        }
        os_sleep(s_monitor.fresh_timegap);
    }
}

// 初始化
void os_monitor_init(void)
{
    os_task_create(os_monitor_entry, "os_monitor", NULL, OS_PRIORITY_LOWEST, OS_TASK_STACK_MIN + 4096);
}

// 是否显示
void os_monitor_display(bool is_open)
{
    s_monitor.is_view = is_open;
}

void os_monitor_current_page_set(uint32_t current_page)
{
    s_monitor.current_page = current_page;
}

uint32_t os_monitor_current_page_get(void)
{
    return s_monitor.current_page;
}

uint32_t os_monitor_total_page_get(void)
{
    return s_monitor.total_pages;
}

bool os_monitor_is_enable(void)
{
    return s_monitor.is_view;
}

void os_monitor_pagesize_set(int per_page)
{
    s_monitor.task_per_page = per_page;
}

uint32_t os_monitor_pagesize_get(void)
{
    return s_monitor.task_per_page;
}

void os_monitor_fresh_timegap_set(uint32_t fresh_timegap)
{
    s_monitor.fresh_timegap = fresh_timegap;
}

uint32_t os_monitor_fresh_timegap_get(void)
{
    return s_monitor.fresh_timegap;
}

float os_cpu_usage_real_get(void)
{
    return s_monitor.cpu_usage_real;
}

float os_cpu_usage_average_get(void)
{
    return s_monitor.cpu_usage_average;
}

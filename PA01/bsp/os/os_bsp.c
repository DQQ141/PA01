#include "./os_bsp.h"

#include "./os_monitor.cc"

/**
 * @brief Get the Run Time Counter Value object
 *
 * @return unsigned long
 */
unsigned long getRunTimeCounterValue(void)
{
    return TIMESTAMP_US;
}

// 演示函数
void os_sleep(uint32_t ms)
{
    if (!is_in_interrupt())
    {
        if ((ms) / portTICK_PERIOD_MS < 1)
            vTaskDelay(1);
        else
            vTaskDelay((ms) / portTICK_PERIOD_MS);
    }
    else
    {
        delay_us((ms) * 1000);
    }
}

// 创建互斥锁
void *os_mutex_create(void)
{
    return xSemaphoreCreateMutex();
}

// 互斥锁创建
void os_mutex_destroy(void **mutex)
{
    ASSERT(*mutex != NULL);
    vSemaphoreDelete((SemaphoreHandle_t)*mutex);
}

// 互斥锁上锁
bool os_mutex_lock(void **mutex, uint32_t timeout_ms)
{
    if (!is_in_interrupt())
    {
        if (*mutex == NULL)
        {
            *mutex = os_mutex_create();
        }
        return xSemaphoreTake((SemaphoreHandle_t)*mutex, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
    }
    else
    {
        return true;
    }
}

// 互斥锁解锁
void os_mutex_unlock(void **mutex)
{
    if (!is_in_interrupt())
    {
        ASSERT(*mutex != NULL);
        xSemaphoreGive((SemaphoreHandle_t)*mutex);
    }
}

/**
 * @brief os产生栈溢出错误
 *
 * @param name 任务名
 */
void os_stack_overflow_hook(char *name)
{
    log_error("running in [%s], stack overflow.", name);
}

void os_assert_failed_hook(char *file, uint32_t line)
{
    log_error("[%s] assert failed in [%s:%d].", os_task_current_name_get(), path_remove(file), line);
}

/**
 * @brief 打印任务创建信息
 *
 * @param is_succuss 是否成功
 * @param content 任务创建信息
 * @param name 任务名
 * @param func 任务函数
 * @param args 任务参数
 * @param priority 任务优先级
 * @param stack_size 任务堆栈大小
 */
void os_print_task_create_info(uint8_t is_succuss, const char *content, const char *name, const char *func, const char *args, const char *priority, const char *stack_size)
{
    if (is_succuss)
    {
        dprint(COLOR_H_WHITE "<%s:%s> \"%s\" %s(arg:%s) [%s] has been created.\r\n" COLOR_L_WHITE,
               content, stack_size, name, func, args, priority);
    }
    else
    {
        dprint(COLOR_H_RED "<%s:%s> \"%s\" %s(arg:%s) [%s] creating failed.\r\n" COLOR_L_WHITE,
               content, stack_size, name, func, args, priority);
    }
}

/**
 * @brief 打印返回信息
 *
 * @param ret 返回值
 */
void os_print_ret(BaseType_t ret)
{
    switch (ret)
    {
    case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
        log_error(DEFINE_TO_STR(errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY));
        break;
    case errQUEUE_BLOCKED:
        log_error(DEFINE_TO_STR(errQUEUE_BLOCKED));
        break;
    case errQUEUE_YIELD:
        log_error(DEFINE_TO_STR(errQUEUE_YIELD));
        break;
    default:
        break;
    }
}

// uint8_t ucDtcm[128 * 1024] __section_dtcm = {0};
uint8_t ucHeap[256 * 1024] __section_axi = {0}; // AXI SRAM
uint8_t ucRam1[128 * 1024] __section_sram1 = {0};
uint8_t ucRam2[128 * 1024] __section_sram2 = {0};
uint8_t ucRam4[64 * 1024] __section_sram4 = {0};
// uint8_t sdram[3 * 1024 * 1024] __section_sdram;

HeapRegion_t configHEAP_5_REGIONS[] = {
    // {ucDtcm, sizeof(ucDtcm)},//概率卡死
    {ucHeap, sizeof(ucHeap)},
    {ucRam1, sizeof(ucRam1)},
    {ucRam2, sizeof(ucRam2)},
    {ucRam4, sizeof(ucRam4)},
    // {sdram, sizeof(sdram)},//屏幕无法使用
    {NULL, 0},
};

void os_heap_init(void)
{
    vPortDefineHeapRegions(configHEAP_5_REGIONS); // 初始化RTOS堆
}

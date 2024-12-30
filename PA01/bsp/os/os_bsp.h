/**
 * @file os_bsp.h
 * @author WittXie
 * @brief 系统板级支持包
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

// OS 堆栈大小
#define OS_HEAP_SIZE configTOTAL_HEAP_SIZE // OS堆大小

// OS配置
#define OS_PRIORITY_HIGHEST (osPriorityRealtime)   // 任务最低高优先级
#define OS_PRIORITY_REALTIME (osPriorityHigh)      // 实时任务优先级
#define OS_PRIORITY_BSP (osPriorityAboveNormal)    // 板级支持包优先级
#define OS_PRIORITY_APP (osPriorityNormal)         // APP优先级
#define OS_PRIORITY_LOWEST (osPriorityBelowNormal) // 任务最低优先级
#define OS_PRIORITY_IDLE (osPriorityLow)           // 空闲任务优先级

// 环境
#include "../bsp_env.h"

// 配置
#define OS_TASK_STACK_MIN 2048 // 任务堆栈最小值

/**
 * @brief 创建任务 xPortGetFreeHeapSize
 * @param _task_handle 任务句柄 TaskHandle_t
 * @param _func 任务函数 void (*_func)(void *task_handle)
 * @param _args 任务参数 void *
 * @param _priority 任务优先级 0~(configMAX_PRIORITIES – 1)
 * @param _stack_size 任务堆栈大小 4的倍数
 *
 */
#define os_task_create(_func, _name, _args, _priority, _stack_size)                                                                         \
    {                                                                                                                                       \
        static TaskHandle_t _task_handle = NULL;                                                                                            \
        BaseType_t ret = pdPASS;                                                                                                            \
                                                                                                                                            \
        ret = xTaskCreate(                                                                                                                  \
            (TaskFunction_t)(_func),                                       /* Function that implements the task. */                         \
            (_name),                                                       /* Text name for the task. */                                    \
            (configSTACK_DEPTH_TYPE)((_stack_size) / sizeof(StackType_t)), /* Number of indexes in the xStack array. */                     \
            (void *)(_args),                                               /* Parameter passed into the task. */                            \
            (UBaseType_t)(_priority),                                      /* Priority at which the task is created. */                     \
            &_task_handle                                                  /* Used to pass out the created task's handle. */                \
        );                                                                                                                                  \
        void os_print_ret(BaseType_t ret);                                                                                                  \
        void os_print_task_create_info(uint8_t is_succuss, const char *content,                                                             \
                                       const char *name, const char *func, const char *args, const char *priority, const char *stack_size); \
        os_print_task_create_info((ret == pdPASS), "dynamic", _name,                                                                        \
                                  DEFINE_TO_STR(_func),                                                                                     \
                                  DEFINE_TO_STR(_args),                                                                                     \
                                  DEFINE_TO_STR(_priority),                                                                                 \
                                  DEFINE_TO_STR(_stack_size));                                                                              \
        os_print_ret(ret);                                                                                                                  \
    }

#define os_task_create_static(_func, _name, _args, _priority, _stack_size)                                                                  \
    {                                                                                                                                       \
        static uint8_t _xStack[(uint32_t)(_stack_size / sizeof(StackType_t)) * sizeof(StackType_t) + sizeof(StackType_t)];                  \
        static StaticTask_t _xTaskBuffer = {0};                                                                                             \
        static TaskHandle_t _task_handle = NULL;                                                                                            \
                                                                                                                                            \
        _task_handle = xTaskCreateStatic(                                                                                                   \
            (TaskFunction_t)(_func),                         /* Function that implements the task. */                                       \
            (_name),                                         /* Text name for the task. */                                                  \
            (uint32_t)((_stack_size) / sizeof(StackType_t)), /* Number of indexes in the xStack array. */                                   \
            (void *)(_args),                                 /* Parameter passed into the task. */                                          \
            (UBaseType_t)(_priority),                        /* Priority at which the task is created. */                                   \
            (StackType_t *)_xStack,                          /* Array to use as the task's stack. */                                        \
            &_xTaskBuffer                                    /* Variable to hold the task's data structure. */                              \
        );                                                                                                                                  \
        void os_print_task_create_info(uint8_t is_succuss, const char *content,                                                             \
                                       const char *name, const char *func, const char *args, const char *priority, const char *stack_size); \
        os_print_task_create_info((_task_handle != NULL), "static", _name,                                                                  \
                                  DEFINE_TO_STR(_func),                                                                                     \
                                  DEFINE_TO_STR(_args),                                                                                     \
                                  DEFINE_TO_STR(_priority),                                                                                 \
                                  DEFINE_TO_STR(_stack_size));                                                                              \
    }

/**
 * @brief 获取任务句柄
 * @param _args 任务参数 void *
 *
 * @return 任务句柄
 */
#define os_task_current_handle_get() xTaskGetCurrentTaskHandle()

/**
 * @brief
 * @param _type 任务参数类型
 * @param _args 任务参数 void *
 *
 * @return 任务参数
 */
#define os_task_args_buff_get(_type, _args) ((_type *)(_args))
#define os_task_args_get(_type, _args, _index) (((_type *)(_args))[_index])

/**
 * @brief 是否在中断中执行
 *
 * @return true 在中断中执行
 * @return false 不在中断中执行
 */
#define is_in_interrupt() ((__get_IPSR() != 0) ? true : false)

/**
 * @brief 等待
 * @param _bool 等待条件指针
 * @param _period 每次延时时间，单位ms
 * @param _ms 延时时间，单位ms
 *
 * @note 该宏会阻塞当前任务，直到等待条件为真或者超时; 如果_ms为0，则不会超时
 */
#define os_sleep_period_until(_bool, _period, _ms)                          \
    {                                                                       \
        uint64_t _timestamp = TIMESTAMP_US;                                 \
        for (;;)                                                            \
        {                                                                   \
            if (_bool)                                                      \
            {                                                               \
                if (_bool)                                                  \
                    break;                                                  \
            }                                                               \
            else if (is_timeout(_timestamp, (_ms) * 1000u) && ((_ms) != 0)) \
            {                                                               \
                log_warn("[%s] is timeout.", DEFINE_TO_STR(_bool));         \
                break;                                                      \
            }                                                               \
            os_sleep(_period);                                              \
        }                                                                   \
    }

/**
 * @brief 等待
 * @param _bool 等待条件指针
 * @param _ms 延时时间，单位ms
 *
 * @note 该宏会阻塞当前任务，直到等待条件为真或者超时; 如果_ms为0，则不会超时
 */
#define os_sleep_until(_bool, _ms)                                          \
    {                                                                       \
        uint64_t _timestamp = TIMESTAMP_US;                                 \
        for (;;)                                                            \
        {                                                                   \
            if (_bool)                                                      \
            {                                                               \
                os_sleep(1);                                                \
                if (_bool)                                                  \
                    break;                                                  \
            }                                                               \
            else if (is_timeout(_timestamp, (_ms) * 1000u) && ((_ms) != 0)) \
            {                                                               \
                os_sleep(1);                                                \
                if (is_timeout(_timestamp, (_ms) * 1000u) && ((_ms) != 0))  \
                {                                                           \
                    log_warn("[%s] is timeout.", DEFINE_TO_STR(_bool));     \
                    break;                                                  \
                }                                                           \
            }                                                               \
            os_sleep(1);                                                    \
        }                                                                   \
    }

/**
 * @brief 获取任务状态
 * @param _task_handle 任务句柄
 * @return eTaskState
 * @note
 *	@arg  eRunning [A task is querying the state of itself, so must be running.]
 *	@arg  eReady [The task being queried is in a read or pending ready list.]
 *	@arg  eBlocked [The task being queried is in the Blocked state.]
 *	@arg  eSuspended [The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out.]
 *	@arg  eDeleted [The task being queried has been deleted, but its TCB has not yet been freed.]
 *	@arg  eInvalid [Used as an 'invalid state' value.]
 */
#define os_task_state_get(_task_handle) (((_task_handle) != NULL) ? eTaskGetState(_task_handle) : NULL)

/**
 * @brief 边界操作
 * @note 该宏会阻塞中断
 * @note 该宏会阻塞调度器
 * @note 该宏会阻塞所有任务
 */
#define os_critical_enter()   \
    {                         \
        taskENTER_CRITICAL(); \
    }

#define os_critical_exit()   \
    {                        \
        taskEXIT_CRITICAL(); \
    }

/**
 * @brief 获取任务句柄
 * @param _name 任务名称
 *
 * @return 任务句柄
 */
#define os_task_find(_name) xTaskGetHandle(_name)

#define os_task_name_get(_task_handle) pcTaskGetName(_task_handle)
#define os_task_current_name_get() pcTaskGetName(os_task_current_handle_get())

/**
 * @brief 任务操作
 * @param _task_handle 任务句柄
 *
 */
#define os_task_suspend(_task_handle) vTaskSuspend(_task_handle)
#define os_task_resume(_task_handle) vTaskResume(_task_handle)
#define os_task_delete(_task_handle)                                      \
    {                                                                     \
        print("task[%s] has been deleted.", pcTaskGetName(_task_handle)); \
        vTaskDelete(_task_handle);                                        \
    }

/**
 * @brief 任务自己退出
 *
 */
#define os_return vTaskDelete(os_task_current_handle_get()) // 替代return

/**
 * @brief 系统延时
 * @param ms 延时时间，单位ms
 *
 */
void os_sleep(uint32_t ms);

/**
 * @brief 从OS全局heap获取内存
 * @param _size 内存大小
 * @return 内存指针
 * @note 该内存需要使用os_free释放
 */
#define os_malloc(_size)               \
    ({                                 \
        void *ptr = NULL;              \
        if (_size > 0)                 \
            ptr = pvPortMalloc(_size); \
        memset(ptr, 0, _size);         \
        ptr;                           \
    })

/**
 * @brief 释放内存
 * @param _pv 内存指针
 * @note 该内存必须是从OS全局heap获取的
 */
#define os_free(_pv)        \
    {                       \
        if ((_pv) != NULL)  \
        {                   \
            vPortFree(_pv); \
            (_pv) = NULL;   \
        }                   \
    }

/**
 * @brief 重新分配内存
 * @param _pv 原内存指针
 * @param _size 新内存大小
 * @return 新内存指针
 * @note 如果_pv为NULL，则等同于os_malloc(_size)；如果_size为0，则等同于os_free(_pv)
 */
#define os_realloc(_pv, _size)                   \
    ({                                           \
        void *new_ptr = NULL;                    \
        if ((_size) > 0)                         \
        {                                        \
            if ((_pv) == NULL)                   \
            {                                    \
                new_ptr = pvPortMalloc(_size);   \
            }                                    \
            else                                 \
            {                                    \
                new_ptr = pvPortMalloc(_size);   \
                if (new_ptr != NULL)             \
                {                                \
                    memcpy(new_ptr, _pv, _size); \
                    vPortFree(_pv);              \
                }                                \
            }                                    \
        }                                        \
        else                                     \
        {                                        \
            os_free(_pv);                        \
        }                                        \
        new_ptr;                                 \
    })

#define os_rely_wait(_flag, _timeout_ms)                                \
    {                                                                   \
        os_sleep_period_until(_flag, 10, _timeout_ms);                  \
        if ((_flag) == false)                                           \
        {                                                               \
            log_error("os_rely_wait failed: %s", DEFINE_TO_STR(_flag)); \
        }                                                               \
    }

/**
 * @brief 互斥锁上锁
 *
 * @param mutex 互斥锁
 * @return true 获取成功
 * @return false 获取失败
 */
bool os_mutex_lock(void **mutex, uint32_t timeout_ms);

/**
 * @brief 互斥锁解锁
 *
 * @param mutex 互斥锁
 */
void os_mutex_unlock(void **mutex);

/**
 * @brief 创建互斥锁
 *
 * @return void* 互斥锁指针
 */
void *os_mutex_create(void);

/**
 * @brief 销毁互斥锁
 *
 * @param mutex 互斥锁指针
 */
void os_mutex_destroy(void **mutex);

// os_monitor
void os_monitor_display(bool is_open);
bool os_monitor_is_enable(void);
void os_monitor_current_page_set(uint32_t current_page);
uint32_t os_monitor_current_page_get(void);
uint32_t os_monitor_total_page_get(void);
uint32_t os_monitor_pagesize_get(void);
void os_monitor_pagesize_set(int per_page);
uint32_t os_monitor_timegap_get(void);
void os_monitor_timegap_set(uint32_t timegap);
void os_monitor_heap_usage(void);

// os_monitor
void os_monitor_init(void);
float os_cpu_usage_real_get(void);
float os_cpu_usage_average_get(void);

// 驱动
#include "../log/log_bsp.h"

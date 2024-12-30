/**
 * @file log.h
 * @author WittXie
 * @brief 日志系统
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 *  //////////////////////////////////////////////////////////
 *  //                      _ooOoo_                         //
 *  //                     o8888888o                        //
 *  //                     88" . "88                        //
 *  //                     (| ^_^ |)                        //
 *  //                     O\  =  /O                        //
 *  //                  ____/`---'\____                     //
 *  //                .'  \\|     |//  `.                   //
 *  //               /  \\|||  :  |||//  \                  //
 *  //              /  _||||| -:- |||||-  \                 //
 *  //              |   | \\\  -  /// |   |                 //
 *  //              | \_|  ''\---/''  |   |                 //
 *  //              \  .-\__  `-`  ___/-. /                 //
 *  //            ___`. .'  /--.--\  `. . ___               //
 *  //          ."" '<  `.___\_<|>_/___.'  >'"".            //
 *  //        | | :  `- \`.;`\ _ /`;.`/ - ` : | |           //
 *  //        \  \ `-.   \_ __\ /__ _/   .-` /  /           //
 *  //  ========`-.____`-.___\_____/___.-`____.-'========   //
 *  //                       `=---='                        //
 *  //  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  //
 *  //        佛祖保佑       永不宕机     永无BUG             //
 *  //////////////////////////////////////////////////////////
 */
#pragma once

// 依赖
#include "./../dds/dds.h"       // 订阅机制
#include "./../ring/ring.h"     // 环形缓冲
#include "./../string/string.h" // 字符串处理
#include "./../time/time.h"     // 时间相关

// 通用接口
#ifndef MALLOC
#error "pleasae define MALLOC and FREE"
#define MALLOC(_size) malloc(_size)
#define FREE(_pv) free(_pv)
#endif

#ifndef TIMESTAMP_US_GET
#error "please define TIMESTAMP_US_GET"
#define TIMESTAMP_US_GET() 0
#endif

#ifndef STACK_MALLOC
#define STACK_MALLOC(_byte_size)     \
    ({                               \
        uint8_t _t_buff[_byte_size]; \
        _t_buff;                     \
    })
#define STACK_FREE(_buff)
#endif

#ifndef MUTEX_LOCK
#define MUTEX_LOCK(_mutex) ((bool)true)
#define MUTEX_UNLOCK(_mutex) ((void)0)
#endif

// 基础
#include "./format.h"

// LOG等级
enum log_level
{
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_TRACE,
    LOG_LEVEL_ALL,
};

typedef struct __log_str
{
    char *data;
    uint32_t length;
} log_str_t;

typedef struct __log
{
    // 参数
    struct
    {
        const char *name;     // 日志名称
        enum log_level level; // LOG等级
        uint32_t buff_size;   // 缓存大小
        const char *filter;   // 日志筛选
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);                            // 初始化
        void (*write)(uint8_t *buff, uint32_t length); // 写入
    } ops;

    // 标志
    union
    {
        uint8_t value;
        struct
        {
            bool is_inited : 1;  // 是否已初始化
            bool is_running : 1; // 是否正在运行
        };
    } flag;

    void *mutex;           // 锁
    uint64_t timestamp_us; // 时间戳
    vdate_t date;          // 日期

    ring_t ring_read;  // 读: 循环缓存队列
    ring_t ring_write; // 写: 循环缓存队列

    // DDS主题
    dds_topic_t INIT;     // 初始化
    dds_topic_t DEINIT;   // 反初始化后
    dds_topic_t POLL;     // 轮询
    dds_topic_t READ;     // 读取
    dds_topic_t REFRESH;  // 刷新
    dds_topic_t PRINT;    // 普通打印，可开启缓存
    dds_topic_t EX_ERROR; // 打印错误
    dds_topic_t EX_WARN;  // 打印警告
    dds_topic_t EX_INFO;  // 打印信息
    dds_topic_t EX_TRACE; // 打印追踪

} log_t;

/**
 * @brief 日志打印
 *
 * @param log 日志指针
 * @param type 日志类型
 * @param color 日志颜色
 * @param format 格式化字符串
 * @param ... 可变参数
 */
#define log_print(_log, _format, ...)                                       \
    {                                                                       \
        if (((_log)->cfg.level >= LOG_LEVEL_TRACE) && ((_format) != NULL))  \
        {                                                                   \
            log_interface_print((_log), _format, ##__VA_ARGS__);            \
        }                                                                   \
        log_dds_publish((_log), &(_log)->EX_TRACE, _format, ##__VA_ARGS__); \
    }

#define log_dprint(_log, _format, ...)                                      \
    {                                                                       \
        if (((_log)->cfg.level >= LOG_LEVEL_TRACE) && ((_format) != NULL))  \
        {                                                                   \
            log_interface_dprint((_log), _format, ##__VA_ARGS__);           \
        }                                                                   \
        log_dds_publish((_log), &(_log)->EX_TRACE, _format, ##__VA_ARGS__); \
    }

#define __log_base_print(_log, _type, _color, _format, ...)                                                             \
    {                                                                                                                   \
        (_log)->timestamp_us = TIMESTAMP_US_GET();                                                                      \
        (_log)->date = timestamp_to_date((_log)->timestamp_us);                                                         \
        log_interface_print(_log, "\r" COLOR_L_##_color #_type FORMAT COLOR_H_##_color _format ASCII_CLEAR_TAIL "\r\n", \
                            FORMAT_CONTENT((&(_log)->date)), ##__VA_ARGS__);                                            \
    }

#define __log_base_dprint(_log, _type, _color, _format, ...)                                                             \
    {                                                                                                                    \
        (_log)->timestamp_us = TIMESTAMP_US_GET();                                                                       \
        (_log)->date = timestamp_to_date((_log)->timestamp_us);                                                          \
        log_interface_dprint(_log, "\r" COLOR_L_##_color #_type FORMAT COLOR_H_##_color _format ASCII_CLEAR_TAIL "\r\n", \
                             FORMAT_CONTENT((&(_log)->date)), ##__VA_ARGS__);                                            \
    }

#define log_print_trace(_log, _format, ...)                                 \
    {                                                                       \
        if (((_log)->cfg.level >= LOG_LEVEL_TRACE) && ((_format) != NULL))  \
        {                                                                   \
            __log_base_print((_log), T, CYAN, _format, ##__VA_ARGS__);      \
        }                                                                   \
        log_dds_publish((_log), &(_log)->EX_TRACE, _format, ##__VA_ARGS__); \
    }

#define log_print_info(_log, _format, ...)                                 \
    {                                                                      \
        if (((_log)->cfg.level >= LOG_LEVEL_INFO) && ((_format) != NULL))  \
        {                                                                  \
            __log_base_print((_log), I, WHITE, _format, ##__VA_ARGS__);    \
        }                                                                  \
        log_dds_publish((_log), &(_log)->EX_INFO, _format, ##__VA_ARGS__); \
    }

#define log_print_warn(_log, _format, ...)                                 \
    {                                                                      \
        if (((_log)->cfg.level >= LOG_LEVEL_WARN) && ((_format) != NULL))  \
        {                                                                  \
            __log_base_print((_log), W, YELLOW, _format, ##__VA_ARGS__);   \
        }                                                                  \
        log_dds_publish((_log), &(_log)->EX_WARN, _format, ##__VA_ARGS__); \
    }

#define log_print_error(_log, _format, ...)                                 \
    {                                                                       \
        if (((_log)->cfg.level >= LOG_LEVEL_ERROR) && ((_format) != NULL))  \
        {                                                                   \
            __log_base_dprint((_log), E, RED, _format, ##__VA_ARGS__);      \
        }                                                                   \
        log_dds_publish((_log), &(_log)->EX_ERROR, _format, ##__VA_ARGS__); \
    }

#define log_debug(_log, _type, _format, ...) log_print_##_type(_log, _format, ##__VA_ARGS__)

/**
 * @brief 日志初始化
 *
 * @param log 日志指针
 */
void log_init(log_t *log);

/**
 * @brief 日志轮调
 *
 */
void log_poll(log_t *log);

/**
 * @brief 日志释放
 *
 */
void log_deinit(log_t *log);

/**
 * @brief 日志刷新
 *
 * @param log
 */
void log_flush(log_t *log);

/**
 * @brief 日志打印钩子
 *
 * @param log 日志指针
 * @param format 格式化字符串
 * @param ... 可变参数
 */
void log_interface_print(log_t *log, const char *format, ...);

/**
 * @brief 日志直接打印钩子
 *
 * @param log 日志指针
 * @param format 格式化字符串
 * @param ... 可变参数
 */
void log_interface_dprint(log_t *log, const char *format, ...);

/**
 * @brief 设备是否初始化
 *
 * @param log 日志指针
 * @return bool true为已初始化，false为未初始化
 */
#define log_is_inited(_log) ((_log)->flag.is_inited)

/**
 * @brief 设备是否正在运行
 *
 * @param log 日志指针
 * @return bool true为正在运行，false为未运行
 */
#define log_is_running(_log) ((_log)->flag.is_running)

/**
 * @brief LOG DDS发布
 *
 * @param log 日志指针
 * @param topic DDS主题
 * @param format 格式化字符串
 * @param ... 可变参数
 */
void log_dds_publish(log_t *log, dds_topic_t *topic, const char *format, ...);

/**
 * @file protocol.h
 * @author WittXie
 * @brief 协议通用驱动
 * @version 0.1
 * @date 2024-10-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 依赖
#include "./../dds/dds.h"   // 订阅机制
#include "./../ring/ring.h" // 环形缓冲

#ifndef MALLOC
#define MALLOC(_size) malloc(_size)
#define FREE(_pv) free(_pv)
#endif

#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

#ifndef ERROR
#define ERROR(_format, ...) ((void)0)
#endif

#ifndef MUTEX_LOCK
#define MUTEX_LOCK(_mutex) ((bool)true)
#define MUTEX_UNLOCK(_mutex) ((void)0)
#endif

#pragma pack(4)
// 数据帧
typedef struct __protocol_frame
{
    uint32_t cmd;         // 命令
    uint32_t addr_src;    // 源地址
    uint32_t addr_dst;    // 目的地址
    uint32_t number;      // 帧号
    uint8_t *data;        // 数据缓存
    uint32_t data_length; // 数据长度
    uint32_t *user_data;  // 用户数据
} protocol_frame_t;
#pragma pack()

// 设备结构体
typedef struct __protocol
{
    // 参数
    struct
    {
        const char *name;   // 名称
        uint16_t buff_size; // 缓冲区大小
        uint16_t frame_min; // 最小帧长度
        uint16_t frame_max; // 最小帧长度
        uint8_t head_code;  // 头部代码
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(void);                                                                    // 初始化
        uint32_t (*pack)(protocol_frame_t *frame, uint8_t *send_buff);                         // 数据打包, 返回需要发送的长度
        uint32_t (*unpack)(protocol_frame_t *frame, uint8_t *recv_buff, uint32_t recv_length); // 数据解包， 返回是否解包长度
        void (*write)(uint8_t *buff, uint32_t length);                                         // 发送数据
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

    void *mutex;       // 互斥锁
    ring_t ring;       // 环形缓冲
    const char *error; // 错误信息

    protocol_frame_t recv_temp_frame; // 临时帧
    uint8_t *recv_buff;               // 缓存
    uint16_t recv_length;             // 接收长度
    uint8_t *send_buff;               // 缓存
    uint16_t send_length;             // 发送长度

    dds_topic_t INIT;        // 初始化完成
    dds_topic_t POLL;        // 论询后
    dds_topic_t RECEIVE;     // 成功接收数据帧
    dds_topic_t SEND;        // 成功发送数据帧
    dds_topic_t RAW_SEND;    // 成功发送原始数据
    dds_topic_t RAW_RECEIVE; // 成功接收原始数据
} protocol_t;

/**
 * @brief 初始化串联PROTOCOL
 *
 * @param protocol 指向设备的结构的指针，该结构包含了串联PROTOCOL操作所需的接口和数据
 */
void protocol_init(protocol_t *protocol);

/**
 * @brief 关闭串联PROTOCOL
 *
 * @param protocol 指向设备的结构的指针
 */
void protocol_deinit(protocol_t *protocol);

/**
 * @brief 论询
 *
 * @param protocol 指向设备的结构的指针
 */
void protocol_poll(protocol_t *protocol);

/**
 * @brief 协议数据读取钩子
 *
 * @param protocol 指向设备的结构体指针
 * @param buff 接收到的数据缓冲区
 * @param length 接收到的数据长度
 */
void protocol_read_hook(protocol_t *protocol, uint8_t *buff, uint32_t length);

/**
 * @brief 发送一帧数据
 *
 * @param protocol 指向设备的结构体指针
 * @param frame 要发送的帧指针
 */
void protocol_send(protocol_t *protocol, protocol_frame_t *frame);

/**
 * @brief 设备是否初始化
 *
 * @param protocol 指向设备的结构体指针
 * @return bool true为已初始化，false为未初始化
 */
#define protocol_is_inited(protocol) ((protocol)->flag.is_inited)

/**
 * @brief 设备是否运行
 *
 * @param protocol 指向设备的结构体指针
 * @return bool true为已运行，false为未运行
 */
#define protocol_is_running(protocol) ((protocol)->flag.is_running)

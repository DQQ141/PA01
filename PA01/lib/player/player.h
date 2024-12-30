/**
 * @file player.h
 * @author WittXie
 * @brief 喇叭驱动
 * @version 0.1
 * @date 2024-09-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

// 依赖
#include "./../dds/dds.h"   // 订阅机制
#include "./../list/list.h" // 链表

// 通用接口
#ifndef ASSERT
#define ASSERT(_bool, ...) ((void)0)
#endif

#ifndef INFO
#define INFO(_format, ...) ((void)0)
#endif

#ifndef TRACE
#define TRACE(_format, ...) ((void)0)
#endif

#ifndef MALLOC
#error "pleasae define MALLOC and FREE"
#define MALLOC(_size) malloc(_size)
#define FREE(_pv) free(_pv)
#endif

typedef enum __player_status
{
    PLAYER_STATUS_IDLE = 0,   // 空闲
    PLAYER_STATUS_PLAYING,    // 播放中
    PLAYER_STATUS_PAUSE,      // 暂停
    PLAYER_STATUS_SET_PAUSE,  // 设置暂停
    PLAYER_STATUS_SET_STOP,   // 设置停止
    PLAYER_STATUS_SET_RESUME, // 设置继续
} player_status_t;

// 资源结构体
typedef struct __voice_t
{
    const char *name;     // 名称
    void *data;           // 数据
    uint8_t data_bits;    // 数据位宽
    uint32_t sample_rate; // 采样率,Hz
    uint32_t length;      // 长度
    void *play_data;      // 实际播放数据
    float volume;         // 当前音量 0-1
} voice_t;

typedef struct __player
{
    // 参数
    struct
    {
        const char *name;  // 名称
        uint8_t data_bits; // 数据位宽
    } cfg;

    // 函数接口
    struct
    {
        void (*init)(voice_t *voice);
        uint32_t (*play)(void *data, uint32_t length); // 会被论询, 返回本次发送的数据长度
        void (*stop)(voice_t *voice);                  // 停止
        void (*pause)(voice_t *voice);                 // 暂停
        void (*enable)(voice_t *voice);                // 暂停
        bool (*is_ready)(voice_t *voice);              // 暂停

    } ops;

    // 数据
    list_t list;
    player_status_t status; // 播放状态
    voice_t *voice;         // 当前数据
    uint32_t remain;        // 剩余数据长度
    uint32_t offset;        // 偏移量
    float volume;           // 音量 0-1
    uint32_t burst_time;    // 突发播放时间限制

    float begin_percent;
    float end_percent; // 突发播放百分比

    // 订阅
    dds_topic_t START;  // 播放开始
    dds_topic_t PAUSE;  // 播放暂停
    dds_topic_t RESUME; // 播放继续
    dds_topic_t STOP;   // 播放停止
    dds_topic_t PLAYED; // 播放完成一个周期
} player_t;

/**
 * @brief 获取播放状态
 *
 * @param _p_player 设备指针
 * @return 获取播放状态
 */
#define player_status_get(_p_player) ((_p_player)->status)

/**
 * @brief 初始化
 *
 * @param player 设备指针
 */
void player_init(player_t *player);

/**
 * @brief 轮询
 *
 * @param player 设备指针
 */
void player_poll(player_t *player);

/**
 * @brief 播放
 *
 * @param player 设备指针
 * @param data 数据
 */
void player_play(player_t *player, voice_t *voice);

/**
 * @brief 暂停
 *
 * @param player 设备指针
 */
void player_pause(player_t *player);

/**
 * @brief 恢复
 *
 * @param player 设备指针
 */
void player_resume(player_t *player);

/**
 * @brief 停止
 *
 * @param player 设备指针
 */
void player_stop(player_t *player);

/**
 * @brief 清空播放
 *
 * @param player 设备指针
 */
void player_clear(player_t *player);

/**
 * @brief 设置音量
 *
 * @param player 设备指针
 * @param volume 音量 0-1
 */
void player_volume_set(player_t *player, float volume);

/**
 * @brief 突发播放
 *
 * @param player 设备指针
 * @param voice 声音
 * @param begin_percent 开始百分比
 * @param end_percent 结束百分比
 *
 * @note 用于播放一小段声音, 比如：0.1，0.3；播放声音的10%到30%的部分
 */
void player_play_burst(player_t *player, voice_t *voice, float begin_percent, float end_percent);

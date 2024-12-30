#include "./player.h"
#undef TRACE
#define TRACE(_format, ...)

// 初始化
void player_init(player_t *player)
{
    ASSERT(player != NULL);
    ASSERT(player->cfg.name != NULL);
    ASSERT(player->cfg.data_bits != 0);
    ASSERT(player->ops.init != NULL);
    ASSERT(player->ops.play != NULL);
    ASSERT(player->ops.stop != NULL);
    ASSERT(player->ops.pause != NULL);
    ASSERT(player->ops.is_ready != NULL);

    // 初始化
    list_init(&player->list);
    player->status = PLAYER_STATUS_IDLE;
    list_node_t *node = list_get_head(&player->list);
    player->ops.init(node->data);
    INFO("[%s] init success.", player->cfg.name);
}

// 音量调节
static void _volume_set(player_t *player, float volume)
{
    voice_t *voice = player->voice;
    if (voice == NULL)
    {
        return;
    }

    if (voice->play_data == NULL)
    {
        size_t play_data_bytes = (voice->data_bits + 7) / 8;
        voice->play_data = MALLOC(voice->length * play_data_bytes);
        ASSERT(voice->play_data != NULL);
    }
    else if (volume == voice->volume)
    {
        return;
    }
    voice->volume = volume;

    for (size_t i = 0; i < voice->length; i++)
    {
        switch (player->cfg.data_bits)
        {
        case 8:
            ((uint8_t *)voice->play_data)[i] = (uint8_t)((float)((uint8_t *)voice->data)[i] * volume) * player->cfg.data_bits / voice->data_bits;
            break;
        case 12:
        case 16:
            ((uint16_t *)voice->play_data)[i] = (uint16_t)((float)((uint16_t *)voice->data)[i] * volume) * player->cfg.data_bits / voice->data_bits;
            break;
        case 24:
        {
            uint32_t sample = ((uint32_t *)voice->data)[i] & 0xFFFFFF; // 24-bit sample
            sample = (uint32_t)((float)sample * volume) * player->cfg.data_bits / voice->data_bits;
            ((uint32_t *)voice->play_data)[i] = sample & 0xFFFFFF;
            break;
        }
        case 32:
            ((uint32_t *)voice->play_data)[i] = (uint32_t)((float)((uint32_t *)voice->data)[i] * volume) * player->cfg.data_bits / voice->data_bits;
            break;
        default:
            log_error("[%s]Unsupported data bits: %d", player->cfg.name, voice->data_bits);
            break;
        }
    }
}

// 轮询
void player_poll(player_t *player)
{
    // 如果播放器为空，直接返回
    if (list_is_empty(&player->list))
    {
        if (player->ops.is_ready(player->voice))
        {
            // 播放完成
            if (player->status != PLAYER_STATUS_IDLE)
            {
                player->ops.stop(player->voice);
                player->status = PLAYER_STATUS_IDLE;
                player->offset = 0; // 重置偏移量
                dds_publish(player, &(player->STOP), NULL);
            }
        }
        return;
    }
    list_node_t *node = list_get_head(&player->list);
    player->voice = node->data;
    voice_t *voice = player->voice;

    // 如果播放器处于暂停状态，直接返回
    if (player->status == PLAYER_STATUS_PAUSE)
        return;

    if (player->status == PLAYER_STATUS_SET_PAUSE)
    {
        player->status = PLAYER_STATUS_PAUSE;
        player->ops.pause(voice);
        dds_publish(player, &(player->PAUSE), voice);
        return;
    }

    if (player->status == PLAYER_STATUS_SET_RESUME)
    {
        player->ops.enable(voice);
        player->status = PLAYER_STATUS_PLAYING;
        dds_publish(player, &(player->RESUME), voice);
        return;
    }

    // 处理停止状态
    if (player->status == PLAYER_STATUS_SET_STOP)
    {
        player->ops.stop(player->voice);
        player->status = PLAYER_STATUS_IDLE;
        player->offset = 0; // 重置偏移量
        dds_publish(player, &(player->STOP), voice);
        return;
    }

    if (player->status == PLAYER_STATUS_IDLE)
    {
        player->ops.enable(voice);
        player->status = PLAYER_STATUS_PLAYING;       // 状态置为播放中
        player->offset = 0;                           // 重置偏移量
        dds_publish(player, &(player->START), voice); // 发布播放中
    }

    _volume_set(player, player->volume); // 调节音量

    uint32_t t_length = voice->length;
    void *t_data = voice->play_data;

    if (list_length_get(&player->list) > 1) // 突发播放
    {
        if (player->begin_percent > 0 && player->end_percent > 0)
        {
            t_length = (uint32_t)(voice->length * (player->end_percent - player->begin_percent));
            t_data = voice->play_data + (uint32_t)(voice->length * player->begin_percent);
        }
    }
    else
    {
        // 重置突发播放
        player->begin_percent = 0;
        player->end_percent = 0;
    }
    TRACE("[%s] play: %d, t_length: %d, list_length: %d, begin_percent: %f, end_percent: %f",
          player->cfg.name, player->offset, t_length, list_length_get(&player->list), player->begin_percent, player->end_percent);

    // 更新偏移量
    uint32_t offset = player->offset;
    if (player->status == PLAYER_STATUS_PLAYING)
    {
        offset += player->ops.play(t_data + player->offset, player->remain); // 播放
    }
    else
    {
        player->ops.stop(player->voice);
    }
    player->offset = offset;
    player->remain = t_length > player->offset ? (t_length - player->offset) : 0;

    // 检查是否播放完成
    if (player->remain == 0)
    {
        player->offset = 0;                            // 重置偏移量
        dds_publish(player, &(player->PLAYED), voice); // 发布播放完一首
        list_destroy(&player->list, node);             // 播放下一首, 释放当前节点
    }
}

// 播放
void player_play(player_t *player, voice_t *voice)
{
    ASSERT(player != NULL);
    ASSERT(voice != NULL);
    if (player == NULL || voice == NULL)
    {
        return;
    }
    // 加入链表
    list_insert_tail(&player->list, list_node_create((voice_t *)voice));
}

// 暂停
void player_pause(player_t *player)
{
    ASSERT(player != NULL);
    if (player == NULL)
    {
        return;
    }

    if (player->status == PLAYER_STATUS_PLAYING)
    {
        player->status = PLAYER_STATUS_SET_PAUSE;
    }
}

// 恢复
void player_resume(player_t *player)
{
    ASSERT(player != NULL);
    if (player == NULL)
    {
        return;
    }

    if (player->status == PLAYER_STATUS_PAUSE)
    {
        player->status = PLAYER_STATUS_SET_RESUME;
    }
}

// 停止
void player_stop(player_t *player)
{
    ASSERT(player != NULL);
    if (player == NULL)
    {
        return;
    }

    if (player->status != PLAYER_STATUS_IDLE)
    {
        player->ops.stop(player->voice);
        player->status = PLAYER_STATUS_SET_STOP;
    }
}

void player_clear(player_t *player)
{
    ASSERT(player != NULL);
    if (player == NULL)
    {
        return;
    }

    list_destroy_all(&player->list);
}

// 音量调节
void player_volume_set(player_t *player, float volume)
{
    ASSERT(player != NULL);
    if (player == NULL)
    {
        return;
    }
    player->volume = volume;
}

// 突发播放
void player_play_burst(player_t *player, voice_t *voice, float begin_percent, float end_percent)
{
    ASSERT(player != NULL);
    ASSERT(voice != NULL);
    if (player == NULL || voice == NULL)
    {
        return;
    }

    if (begin_percent < 0 || begin_percent > 1 || end_percent < 0 || end_percent > 1 || begin_percent > end_percent)
    {
        ERROR("Invalid begin_percent or end_percent.");
        return;
    }

    player->begin_percent = begin_percent;
    player->end_percent = end_percent;

    player_play(player, voice);
}

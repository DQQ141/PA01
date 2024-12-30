#include "./protocol.h"

void protocol_init(protocol_t *protocol)
{
    // 断言
    ASSERT(protocol != NULL);
    ASSERT(protocol->cfg.name != NULL);
    ASSERT(protocol->cfg.buff_size != 0);
    ASSERT(protocol->cfg.frame_min != 0);
    ASSERT(protocol->cfg.frame_max != 0);
    ASSERT(protocol->ops.init != NULL);
    ASSERT(protocol->ops.pack != NULL);
    ASSERT(protocol->ops.unpack != NULL);
    ASSERT(protocol->ops.write != NULL);

    // 内存申请
    uint8_t ret = ring_init(&protocol->ring, protocol->cfg.buff_size);
    if (ret == false)
    {
        ERROR("[%s] ring_init failed.", protocol->cfg.name);
        return;
    }
    protocol->recv_buff = MALLOC(protocol->cfg.frame_max);
    if (protocol->recv_buff == NULL)
    {
        ERROR("[%s] recv_buff malloc failed.", protocol->cfg.name);
        return;
    }
    protocol->send_buff = MALLOC(protocol->cfg.frame_max);
    if (protocol->send_buff == NULL)
    {
        ERROR("[%s] send_buff malloc failed.", protocol->cfg.name);
        return;
    }
    protocol->recv_temp_frame.data = MALLOC(protocol->cfg.frame_max);
    if (protocol->recv_temp_frame.data == NULL)
    {
        ERROR("[%s] recv_temp_frame.data malloc failed.", protocol->cfg.name);
        return;
    }

    // 初始化
    protocol->ops.init();
    protocol->flag.is_inited = true;

    // 初始化完成
    INFO("[%s] init success.", protocol->cfg.name);

    dds_publish(protocol, &protocol->INIT, NULL);
}

// 关闭PROTOCOL
void protocol_deinit(protocol_t *protocol)
{
    ASSERT(protocol != NULL);

    // 清除标志
    protocol->flag.is_inited = false;
    protocol->flag.is_running = false;

    // 释放内存
    ring_deinit(&protocol->ring);

    // 关闭设备完成
    INFO("[%s] deinit success.", protocol->cfg.name);
}

// 论询
void protocol_poll(protocol_t *protocol)
{
    // 检测错误
    if (protocol->error != NULL)
    {
        ERROR("[%s] occurred error: %s.", protocol->cfg.name, protocol->error);
        protocol->error = NULL;
        return;
    }
    dds_publish(protocol, &protocol->POLL, NULL);

    if (protocol->flag.is_inited && protocol->flag.is_running == false)
    {
        protocol->flag.is_running = true;
        ring_clear(&protocol->ring);
    }

    // 数据读取
    protocol->recv_length = ring_peek(&protocol->ring, protocol->recv_buff, protocol->cfg.frame_max);
    if (protocol->recv_length != 0)
    {
        protocol_frame_t frame = {
            .data = protocol->recv_buff,
            .data_length = protocol->recv_length,
        };
        dds_publish(protocol, &protocol->RAW_RECEIVE, &frame);
    }

    if (protocol->recv_length == 0)
    {
        return;
    }

    // 对齐帧头
    for (uint32_t i = 0; i < protocol->recv_length; i++)
    {
        if (protocol->recv_buff[i] == protocol->cfg.head_code)
        {
            ring_discard(&protocol->ring, i);
            break;
        }
    }

    // 数据解析
    for (uint32_t i = 0; i < protocol->recv_length; i++)
    {
        // 查找帧头
        if (protocol->recv_buff[i] != protocol->cfg.head_code)
        {
            continue;
        }

        if (i + protocol->cfg.frame_min > protocol->recv_length)
        {
            break;
        }

        uint32_t unpack_length = protocol->ops.unpack(&protocol->recv_temp_frame, protocol->recv_buff + i, protocol->recv_length - i);
        if (unpack_length == 0 || unpack_length > protocol->recv_length - i)
        {
            continue;
        }

        ring_discard(&protocol->ring, i + unpack_length);
        dds_publish(protocol, &protocol->RECEIVE, &protocol->recv_temp_frame);
        i += unpack_length - 1;
    }
}

// 发送
void protocol_send(protocol_t *protocol, protocol_frame_t *frame)
{
    // 断言
    ASSERT(protocol != NULL);
    ASSERT(frame != NULL);

    if (protocol->flag.is_inited == false)
    {
        ERROR("[%s] protocol_send failed, protocol is not inited.", protocol->cfg.name);
        return;
    }

    if (protocol->send_buff == NULL)
    {
        ERROR("[%s] send_buff is null.", protocol->cfg.name);
        return;
    }

    // 上锁
    if (!MUTEX_LOCK(&protocol->mutex))
    {
        ERROR("[%s] mutex lock failed.", protocol->cfg.name);
        return;
    }
    protocol->send_length = protocol->ops.pack(frame, protocol->send_buff); // 数据打包
    MUTEX_UNLOCK(&protocol->mutex);

    protocol_frame_t raw_frame = {
        .data = protocol->send_buff,
        .data_length = protocol->send_length,
    };

    dds_publish(protocol, &protocol->RAW_SEND, &raw_frame);

    // 上锁
    if (!MUTEX_LOCK(&protocol->mutex))
    {
        ERROR("[%s] mutex lock failed.", protocol->cfg.name);
        return;
    }
    if (protocol->send_length < protocol->cfg.frame_min)
    {
        MUTEX_UNLOCK(&protocol->mutex);
        return;
    }

    // 发送数据
    protocol->ops.write(protocol->send_buff, protocol->send_length);

    // 解锁
    MUTEX_UNLOCK(&protocol->mutex);

    // 发布主题
    dds_publish(protocol, &protocol->SEND, frame);
}

// 数据接收钩子
void protocol_read_hook(protocol_t *protocol, uint8_t *buff, uint32_t length)
{
    if (protocol->flag.is_inited == false)
    {
        return;
    }
    if (protocol->flag.is_running == false)
    {
        return;
    }

    // 检查空间
    if (ring_remain_size(&protocol->ring) < length)
    {
        return;
    }

    // 数据入队
    ring_enqueue(&protocol->ring, buff, length);
}

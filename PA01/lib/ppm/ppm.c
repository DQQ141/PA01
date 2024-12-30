#include "./ppm.h"

void ppm_init(ppm_t *ppm)
{
    ASSERT(ppm != NULL);
    ASSERT(ppm->cfg.name != NULL);
    ASSERT(ppm->cfg.channels != 0);
    ASSERT(ppm->cfg.timer_clk != 0);
    ASSERT(ppm->cfg.period_us != 0);
    ASSERT(ppm->cfg.timer_bits != 0);
    ASSERT(ppm->cfg.n_t0 > 0);
    ASSERT(ppm->cfg.n_t1 > 0);
    ASSERT(ppm->cfg.tolerance_us != 0);
    ASSERT(ppm->ops.init != NULL);
    ASSERT(ppm->ops.deinit != NULL);
    ASSERT(ppm->ops.set_timer != NULL);
    ASSERT(ppm->ops.start != NULL);

    // 计算数据长度
    ppm->send.n_bit_max = ppm->cfg.n_t0 > ppm->cfg.n_t1 // 计算最长脉宽长度
                              ? (ppm->cfg.n_t0 * 2 + 1)
                              : (ppm->cfg.n_t1 * 2 + 1);
    ppm->receive.time_bit_max = (ppm->cfg.n_t0 + 1 + ppm->cfg.n_t1 + 1) * ppm->cfg.period_us; // 计算最长脉宽时间(冗余时间长度)
    ppm->receive.time_t0 = (1 + ppm->cfg.n_t0) * ppm->cfg.period_us;
    ppm->receive.time_t1 = (1 + ppm->cfg.n_t1) * ppm->cfg.period_us;

    ppm->send.pulse_length_max = ppm->cfg.channels * (ppm->send.n_bit_max) + 1 + 1; // 最大脉宽长度 = 通道数据 + 最后一个脉冲 + 重置脉冲
    ppm->data_length = (ppm->cfg.channels - 1) / 8 + 1;

    // 创建内存
    if (ppm->send.pulse == NULL)
    {
        ppm->send.pulse = MALLOC(ppm->send.pulse_length_max * ppm->cfg.timer_bits / 8); // 16位占用2字节，32位占用4字节
        ASSERT(ppm->send.pulse != NULL);
        memset(ppm->send.pulse, 0, ppm->send.pulse_length_max * ppm->cfg.timer_bits / 8);
    }

    if (ppm->send.send_data == NULL)
    {
        ppm->send.send_data = MALLOC(ppm->data_length);
        ASSERT(ppm->send.send_data != NULL);
        memset(ppm->send.send_data, 0, ppm->data_length);
    }

    if (ppm->send.last_send_data == NULL)
    {
        ppm->send.last_send_data = MALLOC(ppm->data_length);
        ASSERT(ppm->send.last_send_data != NULL);
        memset(ppm->send.last_send_data, 0, ppm->data_length);
    }

    if (ppm->receive.receive_data == NULL)
    {
        ppm->receive.receive_data = MALLOC(ppm->data_length);
        ASSERT(ppm->receive.receive_data != NULL);
        memset(ppm->receive.receive_data, 0, ppm->data_length);
    }

    if (ppm->receive.last_receive_data == NULL)
    {
        ppm->receive.last_receive_data = MALLOC(ppm->data_length);
        ASSERT(ppm->receive.last_receive_data != NULL);
        memset(ppm->receive.last_receive_data, 0, ppm->data_length);
    }

    // 初始化
    ppm->ops.init();
    ppm->flag.is_inited = true;

    // 设置定时器周期
    ppm_period_set(ppm, ppm->cfg.period_us);

    // 初始化完成
    INFO("[%s] init success.", ppm->cfg.name);
}

// 关闭PPM
void ppm_deinit(ppm_t *ppm)
{
    ASSERT(ppm != NULL);
    ASSERT(ppm->ops.deinit != NULL);

    // 关闭设备
    ppm->ops.deinit();
    ppm->flag.is_inited = false;
    ppm->flag.is_running = false;

    // 关闭设备完成
    INFO("[%s] deinit success.", ppm->cfg.name);
}

// 论询
void ppm_poll(ppm_t *ppm)
{
    if (ppm->flag.is_inited)
    {
        ppm->flag.is_running = true;
    }
    else
    {
        return;
    }

    // 检测数据变化
    if (memcmp(ppm->send.last_send_data, ppm->send.send_data, ppm->data_length) != 0)
    {
        dds_publish(ppm, &ppm->SEND_DATA_CHANGED, NULL);
        ppm_period_set(ppm, ppm->cfg.period_us); // 重新设置定时器
        memcpy(ppm->send.last_send_data, ppm->send.send_data, ppm->data_length);
    }
    // 发送数据
    ppm->ops.start(ppm->send.pulse, ppm->send.pulse_length);

    // 检测数据变化
    if (memcmp(ppm->receive.last_receive_data, ppm->receive.receive_data, ppm->data_length) != 0)
    {
        // 发布数据变化主题
        dds_publish(ppm, &ppm->RECEIVED_DATA_CHANGED, NULL);
        memcpy(ppm->receive.last_receive_data, ppm->receive.receive_data, ppm->data_length);
    }
}

static void pulse_fill(ppm_t *ppm)
{
    ASSERT(ppm != NULL);
    if (ppm->flag.is_inited == false)
    {
        ERROR("%s pulse_fill failed, ppm is not inited.", ppm->cfg.name);
        return;
    }

    ppm->send.pulse_length = 0;
    // 填装数据
    if (ppm->cfg.timer_bits == 16)
    {
        uint16_t *pulse = (uint16_t *)ppm->send.pulse;
        // 通道数据脉宽
        for (uint32_t i = 0; i < ppm->cfg.channels; i++)
        {
            if (ppm->send.send_data[i / 8] & (1 << (i % 8)))
            {
                // 填装T_BASE脉宽
                pulse[ppm->send.pulse_length++] = ppm->send.pulse_h;
                // 填装T1L脉宽
                for (uint32_t j = 0; j < ppm->cfg.n_t1 * 2; j++)
                {
                    pulse[ppm->send.pulse_length++] = ppm->send.pulse_l;
                }
            }
            else
            {
                // 填装T_BASE脉宽
                pulse[ppm->send.pulse_length++] = ppm->send.pulse_h;
                // 填装T0L脉宽
                for (uint32_t j = 0; j < ppm->cfg.n_t0 * 2; j++)
                {
                    pulse[ppm->send.pulse_length++] = ppm->send.pulse_l;
                }
            }
        }

        // 最后填装短脉宽
        pulse[ppm->send.pulse_length++] = ppm->send.pulse_h;
        pulse[ppm->send.pulse_length++] = ppm->send.pulse_l; // 复位
    }
    else if (ppm->cfg.timer_bits == 32)
    {
        uint32_t *pulse = (uint32_t *)ppm->send.pulse;
        // 通道数据脉宽
        for (uint32_t i = 0; i < ppm->cfg.channels; i++)
        {
            if (ppm->send.send_data[i / 8] & (1 << (i % 8)))
            {
                // 填装T_BASE脉宽
                pulse[ppm->send.pulse_length++] = ppm->send.pulse_h;
                // 填装T1L脉宽
                for (uint32_t j = 0; j < ppm->cfg.n_t1 * 2; j++)
                {
                    pulse[ppm->send.pulse_length++] = ppm->send.pulse_l;
                }
            }
            else
            {
                // 填装T_BASE脉宽
                pulse[ppm->send.pulse_length++] = ppm->send.pulse_h;
                // 填装T0L脉宽
                for (uint32_t j = 0; j < ppm->cfg.n_t0 * 2; j++)
                {
                    pulse[ppm->send.pulse_length++] = ppm->send.pulse_l;
                }
            }
        }

        // 最后填装短脉宽
        pulse[ppm->send.pulse_length++] = ppm->send.pulse_h;
        pulse[ppm->send.pulse_length++] = ppm->send.pulse_l; // 复位
    }
    else
    {
        ERROR("%s timer_bits error.", ppm->cfg.name);
        return;
    }
}

// 设置定时器周期
void ppm_period_set(ppm_t *ppm, uint32_t period_us)
{
    ASSERT(ppm != NULL);
    ASSERT(period_us != 0);

    if (ppm->flag.is_inited == false)
    {
        ERROR("%s pulse_fill failed, ppm is not inited.", ppm->cfg.name);
        return;
    }

    // 记录目标周期（微秒）
    ppm->cfg.period_us = period_us;
    period_us /= 2;

    // 计算定时器最大计数值
    uint32_t max_timer_count = 0xFFFF;
    if (ppm->cfg.timer_bits == 32)
    {
        max_timer_count = 0xFFFFFFFF;
    }

    // 计算合适的分频和周期
    uint32_t prescaler = 1;
    uint32_t timer_count = (ppm->cfg.timer_clk / 1000000) * period_us; // 计算计数值，先不考虑分频器

    // 如果计数值超出最大计数范围，增加分频器
    while (timer_count > max_timer_count)
    {
        prescaler++;
        timer_count = ((ppm->cfg.timer_clk / (prescaler * 1000000)) * period_us);
    }

    // 更新ppm的定时器分频和周期
    ppm->send.timer_prescaler = prescaler - 1;      // 分频器寄存器值通常从0开始
    ppm->send.timer_period = timer_count - 1;       // 周期值
    ppm->send.pulse_h = ppm->send.timer_period + 1; // H脉宽
    ppm->send.pulse_l = 0;                          // L脉宽

    // 设置脉宽
    ppm->ops.set_timer(ppm->send.timer_prescaler, ppm->send.timer_period);

    // 重新填装脉宽数据
    pulse_fill(ppm);
}

// 填装通道数据
void ppm_write(ppm_t *ppm, void *data)
{
    ASSERT(ppm != NULL);
    ASSERT(data != NULL);
    if (ppm->flag.is_inited == false)
    {
        ERROR("%s pulse_fill failed, ppm is not inited.", ppm->cfg.name);
        return;
    }

    // 填装数据
    memcpy(ppm->send.send_data, data, ppm->data_length);
}

// 填装通道数据
void ppm_channel_write(ppm_t *ppm, uint8_t index, bool valid)
{
    ASSERT(ppm != NULL);
    ASSERT(index < ppm->cfg.channels, "index[%u] out of range[%u].", index, ppm->cfg.channels);
    if (ppm->flag.is_inited == false)
    {
        ERROR("%s pulse_fill failed, ppm is not inited.", ppm->cfg.name);
        return;
    }

    // 修改指定位置的数据
    if (valid)
    {
        ppm->send.send_data[index / 8] |= (1 << (index % 8));
    }
    else
    {
        ppm->send.send_data[index / 8] &= ~(1 << (index % 8));
    }
}

// 读取发射通道的数据
bool ppm_send_channel_read(ppm_t *ppm, uint8_t index)
{
    ASSERT(ppm != NULL);
    ASSERT(index < ppm->cfg.channels, "index[%u] out of range[%u].", index, ppm->cfg.channels);

    return ppm->send.send_data[index / 8] & (1 << (index % 8));
}

// 读取接收通道的数据
bool ppm_receive_channel_read(ppm_t *ppm, uint8_t index)
{
    ASSERT(ppm != NULL);
    ASSERT(index < ppm->cfg.channels, "index[%u] out of range[%u].", index, ppm->cfg.channels);

    return ppm->receive.receive_data[index / 8] & (1 << (index % 8));
}

// 读取PPM接收时的上升沿外部中断时间戳；在中断中调用，因此需要尽量简化
void ppm_read_hook(ppm_t *ppm, uint64_t timestamp_us)
{
    uint64_t timegap = timestamp_us - ppm->receive.last_timestamp_us;

    // 滤波
    if (timegap < ppm->cfg.period_us - ppm->cfg.tolerance_us)
        return;

    if (timegap > ppm->receive.time_bit_max)
    {
        ppm->receive.bit_cnt = 0;
    }
    else
    {
        // 数据通道
        if (ppm->receive.bit_cnt < ppm->cfg.channels)
        {
            // 检查脉宽
            if ((timegap > ppm->receive.time_t1 - ppm->cfg.tolerance_us) && (timegap < ppm->receive.time_t1 + ppm->cfg.tolerance_us))
            {
                // 值1，存储
                ppm->receive.receive_data[ppm->receive.bit_cnt / 8] |= (1 << (ppm->receive.bit_cnt % 8));
                ppm->receive.bit_cnt++;
            }
            else if ((timegap > ppm->receive.time_t0 - ppm->cfg.tolerance_us) && (timegap < ppm->receive.time_t0 + ppm->cfg.tolerance_us))
            {
                // 值0，存储
                ppm->receive.receive_data[ppm->receive.bit_cnt / 8] &= ~(1 << (ppm->receive.bit_cnt % 8));
                ppm->receive.bit_cnt++;
            }
        }
    }
    // 记录上次中断时间戳
    ppm->receive.last_timestamp_us = timestamp_us;
}

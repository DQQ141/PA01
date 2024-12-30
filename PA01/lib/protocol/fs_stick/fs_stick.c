#include "./fs_stick.h"

void fs_stick_init(fs_stick_t *fs_stick)
{
    ASSERT(fs_stick != NULL);
    ASSERT(fs_stick->cfg.name != NULL);
    ASSERT(fs_stick->cfg.threshold != 0);
    ASSERT(fs_stick->cfg.try_cnt != 0);
    ASSERT(fs_stick->ops.init != NULL);
    ASSERT(fs_stick->ops.write != NULL);

    // 初始化
    fs_stick->ops.init();
    fs_stick->init_try_cnt = 0;
    fs_stick->flag.value = 0;
}

// 论询
void fs_stick_poll(fs_stick_t *fs_stick)
{
    // 初始化检查
    if (!fs_stick->flag.is_inited)
    {
        if (fs_stick->init_try_cnt < fs_stick->cfg.try_cnt)
        {
            fs_stick->ops.write(0xB1, (uint8_t[]){0x00}, 1); // 获取版本
            fs_stick->ops.write(0xD1, (uint8_t[]){0x00}, 1); // 获取参数

            if (fs_stick->data.param.parameter[0].max != 0)
            {
                fs_stick->flag.is_inited = true;
                INFO("[%s] init success.", fs_stick->cfg.name);
            }
            fs_stick->init_try_cnt++;
        }
        else if (fs_stick->init_try_cnt == fs_stick->cfg.try_cnt)
        {
            ERROR("[%s] init failed.", fs_stick->cfg.name);
            fs_stick->init_try_cnt++;
        }
        return; // 如果未初始化完成，直接返回
    }
    fs_stick->flag.is_running = true;

    // 检查摇杆是否在阈值内
    bool all_sticks_below_threshold =
        (fabs(fs_stick->stick[0].x) < fs_stick->cfg.threshold) &&
        (fabs(fs_stick->stick[0].y) < fs_stick->cfg.threshold) &&
        (fabs(fs_stick->stick[1].x) < fs_stick->cfg.threshold) &&
        (fabs(fs_stick->stick[1].y) < fs_stick->cfg.threshold);

    if (all_sticks_below_threshold)
    {
        dds_publish(fs_stick, &(fs_stick->KEEP_LONG_RELEASE), NULL);
    }
    else
    {
        dds_publish(fs_stick, &(fs_stick->KEEP_LONG_PRESS), NULL);
    }

    // 对比 last_stick 和 stick
    if (memcmp(fs_stick->stick, fs_stick->last_stick, sizeof(fs_stick->stick)) != 0)
    {
        // 检查摇杆状态变化
        bool changed_to_press = false;
        bool changed_to_release = false;
        bool changed = false;

        for (int i = 0; i < 2; i++)
        {
            if ((fabs(fs_stick->stick[i].x) > fs_stick->cfg.threshold && fabs(fs_stick->last_stick[i].x) < fs_stick->cfg.threshold) ||
                (fabs(fs_stick->stick[i].y) > fs_stick->cfg.threshold && fabs(fs_stick->last_stick[i].y) < fs_stick->cfg.threshold))
            {
                changed_to_press = true;
            }
            if ((fabs(fs_stick->stick[i].x) < fs_stick->cfg.threshold && fabs(fs_stick->last_stick[i].x) > fs_stick->cfg.threshold) ||
                (fabs(fs_stick->stick[i].y) < fs_stick->cfg.threshold && fabs(fs_stick->last_stick[i].y) > fs_stick->cfg.threshold))
            {
                changed_to_release = true;
            }
            if (fabs(fs_stick->stick[i].x) > fs_stick->cfg.threshold || fabs(fs_stick->last_stick[i].y) > fs_stick->cfg.threshold)
            {
                changed = true;
            }
        }

        if (changed_to_press)
        {
            dds_publish(fs_stick, &(fs_stick->TO_PRESS), NULL);
        }
        if (changed_to_release)
        {
            dds_publish(fs_stick, &(fs_stick->TO_RELEASE), NULL);
        }
        if (changed)
        {
            dds_publish(fs_stick, &(fs_stick->CHANGED), NULL);
        }
        memcpy(fs_stick->last_stick, fs_stick->stick, sizeof(fs_stick->stick));
    }
}

// 读取数据
void fs_stick_read_hook(fs_stick_t *fs_stick, uint32_t cmd, uint8_t *data, uint32_t data_length)
{
    ASSERT(fs_stick != NULL);
    ASSERT(data != NULL);
    ASSERT(data_length > 0);

    switch (cmd)
    {
    case FS_PROTOCOL_STICK_HALL_CMD: // 霍尔实时值
        if (data_length == FS_PROTOCOL_STICK_HALL_DATA_LENGTH)
        {
            FS_PROTOCOL_STICK_HALL_DATA_SET(&fs_stick->data.hall, data);
            // 计算实时值
            FS_PROTOCOL_STICK_HALL_EXCHANGE(&fs_stick->data.hall, &fs_stick->data.param,
                                            &fs_stick->stick[0].x, &fs_stick->stick[0].y,
                                            &fs_stick->stick[1].x, &fs_stick->stick[1].y);
        }
        else
        {
            // 打印错误信息
            INFO("cmd: 0x%02X data_length: 0x%02X != 0x%02X", cmd, data_length, FS_PROTOCOL_STICK_HALL_DATA_LENGTH);
        }
        break;
    case FS_PROTOCOL_STICK_VERSION_CMD: // 版本信息
        if (data_length == FS_PROTOCOL_STICK_VERSION_DATA_LENGTH)
        {
            FS_PROTOCOL_STICK_VERSION_DATA_SET(&fs_stick->data.version, data);
        }
        else
        {
            // 打印错误信息
            INFO("cmd: 0x%02X data_length: 0x%02X != 0x%02X", cmd, data_length, FS_PROTOCOL_STICK_VERSION_DATA_LENGTH);
        }
        break;
    case FS_PROTOCOL_STICK_PARAM_CMD: // 霍尔参数
        if (data_length == FS_PROTOCOL_STICK_PARAM_DATA_LENGTH)
        {
            FS_PROTOCOL_STICK_PARAM_DATA_SET(&fs_stick->data.param, data);
        }
        else
        {
            // 打印错误信息
            INFO("cmd: 0x%02X data_length: 0x%02X != 0x%02X", cmd, data_length, FS_PROTOCOL_STICK_PARAM_DATA_LENGTH);
        }
        break;
    default:
        INFO("unknown cmd: 0x%02X", cmd);
        PRINT_HEX("", data, data_length);
        break;
    }
}

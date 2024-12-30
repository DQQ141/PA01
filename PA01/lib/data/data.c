#include "./data.h"

// 数据格式
#pragma pack(4)
typedef struct __data_format
{
    uint64_t build_time; // 编译时间戳
    uint16_t check_code; // dcb校验码
} data_format_t;
#pragma pack()

// 数据初始化
void data_init(data_t *data)
{
    ASSERT(data != NULL);
    ASSERT(data->cfg.name != NULL);
    ASSERT(data->cfg.length > 0);
    ASSERT(data->ops.init != NULL);
    ASSERT(data->ops.check != NULL);
    ASSERT(data->ops.read != NULL);
    ASSERT(data->ops.write != NULL);

    data->cache = MALLOC(data->cfg.length);
    ASSERT(data->cache != NULL);
}

static void _data_save(data_t *data)
{

    data_format_t df = {0};
    vdate_t date = build_date_get();

    // 设置DCB
    df.build_time = date_to_timestamp(&date);

    // 准备数据 ：DCB + 数据
    df.check_code = data->ops.check((uint8_t *)data->cfg.user_data, data->cfg.length);
    memcpy(data->cache, &df, sizeof(df)); // 转移到缓存
    memcpy(data->cache + sizeof(df), data->cfg.user_data, data->cfg.length);

    // 写入数据, 数据存储
    data->ops.write(data->cache, sizeof(df) + data->cfg.length);
}

// 数据保存
void data_save(data_t *data)
{
    ASSERT(data != NULL);
    if (!MUTEX_LOCK(&data->mutex))
    {
        ERROR("%s lock failed.", data->cfg.name);
        return;
    }

    _data_save(data);
    MUTEX_UNLOCK(&data->mutex);
}

// 数据加载
void data_load(data_t *data)
{
    ASSERT(data != NULL);
    if (!MUTEX_LOCK(&data->mutex))
    {
        ERROR("%s lock failed.", data->cfg.name);
        return;
    }

    data_format_t df = {0};
    vdate_t date = build_date_get();
    uint16_t ret;

    // 读取数据到cache：DCB + 数据
    data->ops.read(data->cache, sizeof(df) + data->cfg.length);

    // 转移数据
    memcpy(&df, data->cache, sizeof(df));                                    // DCB
    memcpy(data->cfg.user_data, data->cache + sizeof(df), data->cfg.length); // 数据

    // 校验编译版本
    if (df.build_time != date_to_timestamp(&date))
    {
        WARN("%s build version not match, saved: %llu, current: %llu", data->cfg.name, df.build_time, date_to_timestamp(&date));
        memset(data->cfg.user_data, 0, data->cfg.length); // 清空数据
        _data_save(data);                                 // 保存清空数据

        MUTEX_UNLOCK(&data->mutex);
        return;
    }

    // 校验数据
    ret = data->ops.check((uint8_t *)data->cfg.user_data, data->cfg.length);
    if (ret != df.check_code)
    {
        WARN("%s check failed, saved: 0x%04X, current: 0x%04X", data->cfg.name, df.check_code, ret);
        WARN("%s data will be cleared.", data->cfg.name);
        memset(data->cfg.user_data, 0, data->cfg.length); // 清空数据
        _data_save(data);                                 // 保存清空数据

        MUTEX_UNLOCK(&data->mutex);
        return;
    }

    MUTEX_UNLOCK(&data->mutex);
}

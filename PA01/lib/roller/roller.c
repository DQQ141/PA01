#include "./roller.h"

// 初始化
void roller_init(roller_t *roller)
{
    ASSERT(roller != NULL);
    ASSERT(roller->cfg.name != NULL);
    ASSERT(roller->ops.init != NULL);
    ASSERT(roller->ops.read != NULL);

    // 初始化
    roller->ops.init();
    roller->flag.is_inited = true;
    INFO("[%s] init success.", roller->cfg.name);
}

// 轮询
void roller_poll(roller_t *roller)
{
    if (roller->flag.is_inited == false)
    {
        return;
    }
    roller->flag.is_running = true;

    uint64_t current_timestamp = TIMESTAMP_US_GET();
    int32_t step = roller->ops.read();
    if (step != 0)
    {
        roller->original_step += step;             // 更新步数
        roller->timestamp_end = current_timestamp; // 记录最后时间
    }
    else
    {
        if (current_timestamp - roller->timestamp_end > roller->cfg.filter_us)
        {
            if (roller->original_step != 0)
            {
                roller->speed = roller->original_step * 1000000.0f / (current_timestamp - roller->timestamp_start); // 计算速度

                // 分频
                roller->speed = roller->speed / (roller->cfg.div + 1);
                roller->step = roller->original_step / (roller->cfg.div + 1);

                if (roller->step != 0)
                {
                    // 发布更新
                    dds_publish(roller, &roller->CHANGED, &roller->step);
                    roller->original_step -= roller->step * (roller->cfg.div + 1);
                    roller->step = 0;
                }
            }
            roller->speed = 0.0f;
        }

        roller->timestamp_start = current_timestamp;
    }
}

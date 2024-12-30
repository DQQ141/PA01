#include "./player_bsp.h"

// 驱动加载
#include "./../../lib/player/player.c"

// 资源加载
#include "./res/voice_dada.cc"
#include "./res/voice_entry.cc"
#include "./res/voice_error.cc"
#include "./res/voice_exit.cc"
#include "./res/voice_msg.cc"
#include "./res/voice_select.cc"
#include "./res/voice_welcome.cc"

// 端口加载
#include "./port/voice.cc" // 声音驱动

// player组
player_t *g_player_group[] = {
    &g_player,
};
const uint32_t PLAYER_GROUP_SIZE = countof(g_player_group);

// 生成按键音效的函数
static void generate_voice(float freq, uint32_t sample_rate, size_t buffer_length)
{
    uint16_t *buffer = (uint16_t *)os_malloc(buffer_length * sizeof(uint16_t));
    // 计算每个样本的时间间隔
    float time_per_sample = 1.0 / sample_rate;

    // 12位数据宽度，最大值为4095
    uint16_t max_value = 4095;
    for (size_t i = 0; i < buffer_length; i++)
    {
        float t = i * time_per_sample;
        float sample = (max_value / 2.0) * (1.0 + sin(2.0 * M_PI * freq * t));
        buffer[i] = (uint16_t)sample;
    }

    // 打印成数组
    print("{\r\n");
    for (int i = 0; i < buffer_length; i++)
    {
        print("%d, ", buffer[i]);
    }
    print("};\r\n");

    os_free(buffer);
}

// player轮询任务
static void player_entry(void *args)
{
    for (;;)
    {
        os_sleep(10);
        for (int i = 0; i < PLAYER_GROUP_SIZE; i++)
        {
            player_poll(g_player_group[i]);
        }
    }
}

// BSP初始化
void player_bsp_init(void)
{
    for (int i = 0; i < PLAYER_GROUP_SIZE; i++)
    {
        player_init(g_player_group[i]);
    }
    os_task_create(player_entry, "player", NULL, OS_PRIORITY_LOWEST, OS_TASK_STACK_MIN + 1024);
}

#include "./fs_inrm303.h"

// 指令
#include "./../../lib/protocol/fs_inrm303/cmd/excmd.cc"   //拓展指令
#include "./../../lib/protocol/fs_inrm303/cmd/info.cc"    //信息指令
#include "./../../lib/protocol/fs_inrm303/cmd/mode.cc"    //模式指令
#include "./../../lib/protocol/fs_inrm303/cmd/pair.cc"    //对码指令
#include "./../../lib/protocol/fs_inrm303/cmd/ready.cc"   //准备指令
#include "./../../lib/protocol/fs_inrm303/cmd/rf_test.cc" //测试模式指令
#include "./../../lib/protocol/fs_inrm303/cmd/status.cc"  //状态指令

// 初始化
void fs_inrm303_init(fs_inrm303_t *fs_inrm303)
{
    ASSERT(fs_inrm303 != NULL);
    ASSERT(fs_inrm303->cfg.name != NULL);
    ASSERT(fs_inrm303->cfg.try_cnt != 0);
    ASSERT(fs_inrm303->cfg.channel_size != 0 && fs_inrm303->cfg.channel_size <= 32);
    ASSERT(fs_inrm303->ops.init != NULL);
    ASSERT(fs_inrm303->ops.write != NULL);

    // 初始化数据
    memset(&fs_inrm303->data, 0, sizeof(fs_inrm303->data));
    fs_inrm303->init_try_cnt = 0;
    fs_inrm303->flag.value = 0;

    // 初始化
    fs_inrm303->ops.init();

    // 初始化指令响应
    fs_inrm303_init_cmd_info(&g_fs_inrm303);
    fs_inrm303_init_cmd_mode(&g_fs_inrm303);
    fs_inrm303_init_cmd_pair(&g_fs_inrm303);
    fs_inrm303_init_cmd_ready(&g_fs_inrm303);
    fs_inrm303_init_cmd_status(&g_fs_inrm303);
    fs_inrm303_init_cmd_rf_test(&g_fs_inrm303);
    fs_inrm303_init_excmd(&g_fs_inrm303);

    fs_inrm303->timestamp.ready = TIMESTAMP_US_GET();

    dds_publish(fs_inrm303, &fs_inrm303->INIT, NULL);
}

// 论询
void fs_inrm303_poll(fs_inrm303_t *fs_inrm303)
{
    // 初始化检查
    if (!fs_inrm303->flag.is_inited)
    {
        if (is_timeout(fs_inrm303->timestamp.ready, 500000))
        {
            fs_inrm303->timestamp.ready = TIMESTAMP_US_GET();

            if (fs_inrm303->init_try_cnt < fs_inrm303->cfg.try_cnt)
            {
                fs_inrm303_update(fs_inrm303);                                                                         // 更新数据
                fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_READ, FS_INRM303_CID_VERSION_INFO), NULL, 0); // 获取信息
                fs_inrm303->ops.write(FS_INRM303_CMD_GET(FS_INRM303_TYPE_ACK_NONE, FS_INRM303_CID_STATUS), NULL, 0);   // 回复RF状态

                if (fs_inrm303_is_ready(&fs_inrm303->data.ready) &&
                    (fs_inrm303->data.info.product_id != 0) &&
                    (fs_inrm303->data.status.status != 0))
                {
                    fs_inrm303->flag.is_inited = true;
                    INFO("[%s] init success.", fs_inrm303->cfg.name);
                }
                fs_inrm303->init_try_cnt++;
            }
            else if (fs_inrm303->init_try_cnt == fs_inrm303->cfg.try_cnt)
            {
                ERROR("[%s] init failed. ready[%d], product_id[%d], status[%d]",
                      fs_inrm303->cfg.name,
                      fs_inrm303->data.ready,
                      fs_inrm303->data.info.product_id,
                      fs_inrm303->data.status.status);
                fs_inrm303->init_try_cnt++;
            }
            return; // 如果未初始化完成，直接返回
        }
    }
    // 一切准备就绪，开始轮询
    fs_inrm303->flag.is_running = true;

    if (!fs_inrm303->flag.is_running)
    {
        return;
    }

    dds_publish(fs_inrm303, &fs_inrm303->POLL, NULL);
}

// 读取数据
void fs_inrm303_read_hook(fs_inrm303_t *fs_inrm303, uint32_t cmd, uint8_t *data, uint32_t data_length)
{
    ASSERT(fs_inrm303 != NULL);
    ASSERT(data != NULL);
    ASSERT(data_length > 0);
    fs_inrm303_received_t received = {0};
    received.cmd = cmd;
    received.data = data;
    received.data_length = data_length;
    dds_publish(fs_inrm303, &fs_inrm303->RECEIVED, &received);
}

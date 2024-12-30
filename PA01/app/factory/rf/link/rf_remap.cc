/**
 * @file rf_remap.cc
 * @author WittXie
 * @brief 通道映射
 * @version 0.1
 * @date 2024-12-04
 * @note 字节长度=2（signed short） * 通道个数
         (通道数据范围-15000~15000)
         -10000：1000us
         0:1500us
         10000:2000us
         右摇杆左右：对应接收机或测试架CH1，向左输出1000，中间1500，向右输出2000±5uS
         右摇杆上下：对应接收机或测试架CH2，向下输出1000，中间1500，向上输出2000±5uS
         左摇杆上下：对应接收机或测试架CH3，向下输出1000，中间1500，向上输出2000±5uS
         左摇杆左右：对应接收机或测试架CH4，向左输出1000，中间1500，向右输出2000±5uS
         S1旋钮：对应接收机或测试架CH5，顺时钟旋转增大，逆时针旋转减小，范围1000~2000±5uS
         S2旋钮：对应接收机或测试架CH6，顺时钟旋转增大，逆时针旋转减小，范围1000~2000±5uS
         SA开关：对应接收机或测试架CH7，按下2000，弹起1000
         SB开关：对应接收机或测试架CH8，向上1000，中间1500，向下2000
         SC开关：对应接收机或测试架CH9，向上1000，中间1500，向下2000
         SD开关：对应接收机或测试架CH10，按下2000，弹起1000
         SE按键：对应接收机或测试架CH11，按下2000，弹起1000
         SF按键：对应接收机或测试架CH12，按下2000，弹起1000
         T1~T4微调CH1~CH4，按一次步进5Us，范围-150~150
 * @copyright Copyright (c) 2024
 *
 */
#include "./../../factory_app.h"

static void rf_remap_entry(void *args)
{
    os_rely_wait(g_data_factory.cfg.user_data != NULL, 10000);
    for (;;)
    {
        os_sleep(10);
        // 右摇杆
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 1, (1500 + 500 * g_fs_stick.stick[1].x + g_factory.data.stick_shift[3]));
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 2, (1500 + 500 * -g_fs_stick.stick[1].y + g_factory.data.stick_shift[1]));

        // 左摇杆
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 3, (1500 + 500 * -g_fs_stick.stick[0].y + g_factory.data.stick_shift[0]));
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 4, (1500 + 500 * g_fs_stick.stick[0].x + g_factory.data.stick_shift[2]));

        // 旋钮
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 5, 2000 - g_adc_pot_left * 1000);
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 6, 2000 - g_adc_pot_right * 1000);

        // 段位开关
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 8, g_adc_sw_left < 0.3 ? 1000 : (g_adc_sw_left < 0.7 ? 1500 : 2000));
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 9, g_adc_sw_right < 0.3 ? 1000 : (g_adc_sw_right < 0.7 ? 1500 : 2000));

        // 掉电保护
        time_invoke(1000000, {
            if (CMP_PREV(g_factory.data) != 0)
            {
                data_save(&g_data_factory);
            }
        });
    }
}

static void rf_remap_trxp_btn_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    int16_t *tn = (int16_t *)userdata;
    if (tn)
    {
        *tn -= 5;
        if (*tn < -150)
        {
            *tn = -150;
        }
    }
}

static void rf_remap_trxn_btn_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    int16_t *tn = (int16_t *)userdata;
    if (tn)
    {
        *tn += 5;
        if (*tn > 150)
        {
            *tn = 150;
        }
    }
}
static void rf_remap_btn_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    gpio_t *gpio = (gpio_t *)device;
    int16_t value = (int32_t)userdata;
    if (gpio == &g_btn_slw_left)
    {
        log_trace("btn_slw_left: %d", value);
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 7, value);
    }
    else if (gpio == &g_btn_right)
    {
        log_trace("btn_right: %d", value);
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 10, value);
    }
    else if (gpio == &g_btn_back_left)
    {
        log_trace("btn_back_left: %d", value);
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 11, value);
    }
    else if (gpio == &g_btn_back_right)
    {
        log_trace("btn_back_right: %d", value);
        fs_inrm303_chnnel_us_set(&g_fs_inrm303, 12, value);
    }
}

// 初始化
static void rf_remap_init(void)
{
    // 摇杆
    os_task_create(rf_remap_entry, "rf_remap", NULL, OS_PRIORITY_APP, OS_TASK_STACK_MIN);

    // 摇杆增益
    dds_subcribe(&g_exbtn_tr1n.PRESSED, DDS_PRIORITY_NORMAL, rf_remap_trxn_btn_callback, &g_factory.data.stick_shift[0]);
    dds_subcribe(&g_exbtn_tr2n.PRESSED, DDS_PRIORITY_NORMAL, rf_remap_trxn_btn_callback, &g_factory.data.stick_shift[1]);
    dds_subcribe(&g_exbtn_tr3p.PRESSED, DDS_PRIORITY_NORMAL, rf_remap_trxn_btn_callback, &g_factory.data.stick_shift[2]);
    dds_subcribe(&g_exbtn_tr4p.PRESSED, DDS_PRIORITY_NORMAL, rf_remap_trxn_btn_callback, &g_factory.data.stick_shift[3]);
    dds_subcribe(&g_exbtn_tr1p.PRESSED, DDS_PRIORITY_NORMAL, rf_remap_trxp_btn_callback, &g_factory.data.stick_shift[0]);
    dds_subcribe(&g_exbtn_tr2p.PRESSED, DDS_PRIORITY_NORMAL, rf_remap_trxp_btn_callback, &g_factory.data.stick_shift[1]);
    dds_subcribe(&g_exbtn_tr3n.PRESSED, DDS_PRIORITY_NORMAL, rf_remap_trxp_btn_callback, &g_factory.data.stick_shift[2]);
    dds_subcribe(&g_exbtn_tr4n.PRESSED, DDS_PRIORITY_NORMAL, rf_remap_trxp_btn_callback, &g_factory.data.stick_shift[3]);

    // 左右按钮
    dds_subcribe(&g_btn_slw_left.TO_INVALID, DDS_PRIORITY_NORMAL, rf_remap_btn_callback, (void *)1000);
    dds_subcribe(&g_btn_slw_left.TO_VALID, DDS_PRIORITY_NORMAL, rf_remap_btn_callback, (void *)2000);
    dds_subcribe(&g_btn_right.TO_INVALID, DDS_PRIORITY_NORMAL, rf_remap_btn_callback, (void *)1000);
    dds_subcribe(&g_btn_right.TO_VALID, DDS_PRIORITY_NORMAL, rf_remap_btn_callback, (void *)2000);

    // 背后按钮
    dds_subcribe(&g_btn_back_left.TO_INVALID, DDS_PRIORITY_NORMAL, rf_remap_btn_callback, (void *)1000);
    dds_subcribe(&g_btn_back_left.TO_VALID, DDS_PRIORITY_NORMAL, rf_remap_btn_callback, (void *)2000);
    dds_subcribe(&g_btn_back_right.TO_INVALID, DDS_PRIORITY_NORMAL, rf_remap_btn_callback, (void *)1000);
    dds_subcribe(&g_btn_back_right.TO_VALID, DDS_PRIORITY_NORMAL, rf_remap_btn_callback, (void *)2000);
}

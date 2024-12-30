#include "./bsp.h"

// 环境
#include "./bsp_env.h"

// 板级支持包处理接口
#include "./bsp_base.cc"  // 基础函数
#include "./bsp_error.cc" // 错误处理
#include "./bsp_irq.cc"   // 中断处理

void app_init(void);
static void init_entry(void *args)
{
    // 注册模块
    dds_init_create((dds_task_fn_t)time_bsp_init, NULL, DDS_PRIORITY_SUPER);        // 初始化时间戳
    dds_init_create((dds_task_fn_t)log_bsp_init, NULL, DDS_PRIORITY_SUPER);         // 日志
    dds_init_create((dds_task_fn_t)os_monitor_init, NULL, DDS_PRIORITY_SUPER);      // RTOS
    dds_init_create((dds_task_fn_t)reset_reason_check, NULL, DDS_PRIORITY_NORMAL);  // 检查复位原因
    dds_init_create((dds_task_fn_t)gc9307c_bsp_init, NULL, DDS_PRIORITY_NORMAL);    // GC9307C LCD屏幕
    dds_init_create((dds_task_fn_t)crc_bsp_init, NULL, DDS_PRIORITY_NORMAL);        // CRC
    dds_init_create((dds_task_fn_t)serial_led_bsp_init, NULL, DDS_PRIORITY_NORMAL); // 串口灯
    dds_init_create((dds_task_fn_t)roller_bsp_init, NULL, DDS_PRIORITY_NORMAL);     // 滚轮
    dds_init_create((dds_task_fn_t)aw9523b_bsp_init, NULL, DDS_PRIORITY_NORMAL);    // IO拓展芯片 AW9523B
    dds_init_create((dds_task_fn_t)gpio_bsp_init, NULL, DDS_PRIORITY_NORMAL);       // GPIO模块
    dds_init_create((dds_task_fn_t)player_bsp_init, NULL, DDS_PRIORITY_NORMAL);     // 播放器
    dds_init_create((dds_task_fn_t)adc_bsp_init, NULL, DDS_PRIORITY_NORMAL);        // ADC
    dds_init_create((dds_task_fn_t)lsm6dsdtr_bsp_init, NULL, DDS_PRIORITY_NORMAL);  // 陀螺仪 LSM6DS-DTR
    dds_init_create((dds_task_fn_t)lvgl_bsp_init, NULL, DDS_PRIORITY_NORMAL);       // lvgl
    dds_init_create((dds_task_fn_t)ppm_bsp_init, NULL, DDS_PRIORITY_NORMAL);        // PPM
    dds_init_create((dds_task_fn_t)protocol_bsp_init, NULL, DDS_PRIORITY_LOW);      // 协议
    dds_init_create((dds_task_fn_t)app_init, NULL, DDS_PRIORITY_LOWEST);            // APP初始化
    wdg_bsp_init();                                                                 // 看门狗

    for (;;)
    {
        dds_poll(); // 执行空闲程序
        os_sleep(10);
    }
    // 不能退出，FreeRTOS退出父任务后会删除子任务的TCB的索引，导致内存异常
}

void bsp_init(void)
{
    // RTOS之前先用静态任务创建一个，防止系统异常退出后任务被删除
    // 此处RTOS还未初始化，必须用静态创建
    os_task_create_static(init_entry, "dds_idle", NULL, OS_PRIORITY_IDLE, OS_TASK_STACK_MIN + 4096);
}

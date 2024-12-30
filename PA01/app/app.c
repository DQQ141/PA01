#include "./app.h"

void app_init(void)
{
    void factory_app_init(); // 工厂模式
    dds_init_create((dds_task_fn_t)factory_app_init, NULL, DDS_PRIORITY_NORMAL);

    void os_monitor_app_init(); // 系统监控
    dds_init_create((dds_task_fn_t)os_monitor_app_init, NULL, DDS_PRIORITY_NORMAL);

    void test_app_init(); // 测试用例
    dds_init_create((dds_task_fn_t)test_app_init, NULL, DDS_PRIORITY_NORMAL);

    void script_app_init(void); // 脚本
    dds_init_create((dds_task_fn_t)script_app_init, NULL, DDS_PRIORITY_NORMAL);
}

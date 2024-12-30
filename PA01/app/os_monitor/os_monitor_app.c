#include "./os_monitor_app.h"

static void button_os_display_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    os_monitor_display(!os_monitor_is_enable());
}
static void button_os_pageup_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    if (os_monitor_is_enable() && os_monitor_current_page_get() < os_monitor_total_page_get())
        os_monitor_current_page_set(os_monitor_current_page_get() + 1);
}
static void button_os_pagedown_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    if (os_monitor_is_enable() && os_monitor_current_page_get() != 0)
        os_monitor_current_page_set(os_monitor_current_page_get() - 1);
}
static void button_clear_callback(void *device, dds_topic_t *topic, void *arg, void *userdata)
{
    print(ASCII_CLEAR);
}
void os_monitor_app_init(void)
{
#if DEBUG == 1
    dds_subcribe(&g_btn_power.PRESSED, DDS_PRIORITY_NORMAL, &button_os_display_callback, NULL); // 显示监控状态
    dds_subcribe(&g_btn_right.PRESSED, DDS_PRIORITY_NORMAL, &button_clear_callback, NULL);      // 清屏

    dds_subcribe(&g_exbtn_tr4n.PRESSED, DDS_PRIORITY_NORMAL, &button_os_pagedown_callback, NULL); // 翻页 -
    dds_subcribe(&g_exbtn_tr4p.PRESSED, DDS_PRIORITY_NORMAL, &button_os_pageup_callback, NULL);   // 翻页 +

    os_monitor_heap_usage(); // 打印堆大小
#endif
}

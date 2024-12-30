#include "./factory_app.h"

factory_app_t g_factory = {0};

static char s_hw_version[8];
void factory_app_init(void)
{
    g_factory.status.is_running = DEBUG;

    // 读取ADC g_adc_hw_version, 换算： 1.0 ~ 0 => v1.0 ~ v1.8
    float hw_version = round(1.0f + (1.0f - g_adc_hw_version) * (1.8f - 1.0f));
    sprintf(s_hw_version, "v%.1f", hw_version);
    g_factory.hw_version = s_hw_version;
    g_factory.sw_version = SW_VERSION;

    void factory_button_init(void);
    factory_button_init(); // 按键

    void factory_power_init(void);
    factory_power_init(); // 电源

    void factory_rf_init(void);
    factory_rf_init(); // RF

    void factory_ui_init(void);
    factory_ui_init(); // 界面
}

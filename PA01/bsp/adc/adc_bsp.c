#include "./adc_bsp.h"

float g_adc_usb = 0.0f;
lp_filter_t g_filter_usb = {0};
float g_adc_battery = 0.0f;
lp_filter_t g_filter_battery = {0};
float g_adc_pot_left = 0.0f;
lp_filter_t g_filter_pot_left = {0};
float g_adc_pot_right = 0.0f;
lp_filter_t g_filter_pot_right = {0};
float g_adc_sw_left = 0.0f;
lp_filter_t g_filter_sw_left = {0};
float g_adc_sw_right = 0.0f;
lp_filter_t g_filter_sw_right = {0};
float g_adc_hw_version = 0.0f;
lp_filter_t g_filter_hw_version = {0};

static bool s_adc_is_inited = false;
static bool s_adc_is_running = false;
static const uint16_t ADC_PERIOD_MS = 20; // 周期20ms

static float *adc1_ptr[] = {
    &g_adc_usb,
    &g_adc_battery,
    &g_adc_pot_left,
    &g_adc_pot_right,
    &g_adc_sw_left,
    &g_adc_hw_version,
};
static const uint16_t ADC1_NUM = countof(adc1_ptr);
static uint16_t adc1_buff[countof(adc1_ptr)] __section_sdram;
static lp_filter_t *g_filter1_group[countof(adc1_ptr)] = {
    &g_filter_usb,
    &g_filter_battery,
    &g_filter_pot_left,
    &g_filter_pot_right,
    &g_filter_sw_left,
    &g_filter_hw_version,
};

static float *adc3_ptr[] = {
    &g_adc_sw_right,
};
static const uint16_t ADC3_NUM = countof(adc3_ptr);
static uint16_t adc3_buff[countof(adc3_ptr)] __section_sdram;
static lp_filter_t *g_filter3_group[countof(adc3_ptr)] = {
    &g_filter_sw_right,
};

static void adc_init_entry(void *args)
{
    // ADC1初始化
    memset(adc1_buff, 0, sizeof(adc1_buff)); // SDRAM 需要手动清零
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_buff, ADC1_NUM);

    // ADC3初始化
    memset(adc3_buff, 0, sizeof(adc3_buff)); // SDRAM 需要手动清零
    HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adc3_buff, ADC3_NUM);
    os_sleep(ADC_PERIOD_MS);

    lp_filter_init(&g_filter_usb, 0.5f, 1000.0f / ADC_PERIOD_MS, 3);
    lp_filter_init(&g_filter_battery, 0.5f, 1000.0f / ADC_PERIOD_MS, 3);
    lp_filter_init(&g_filter_hw_version, 0.5f, 1000.0f / ADC_PERIOD_MS, 3);
    lp_filter_init(&g_filter_pot_left, 2.0f, 1000.0f / ADC_PERIOD_MS, 1);
    lp_filter_init(&g_filter_pot_right, 2.0f, 1000.0f / ADC_PERIOD_MS, 1);
    lp_filter_init(&g_filter_sw_left, 2.0f, 1000.0f / ADC_PERIOD_MS, 1);
    lp_filter_init(&g_filter_sw_right, 2.0f, 1000.0f / ADC_PERIOD_MS, 1);

    s_adc_is_inited = true;
    log_info("adc init done.");
    os_return;
}

static void adc_entry(void *args)
{
    os_rely_wait(s_adc_is_inited == true, 6000);
    if (s_adc_is_inited == false)
    {
        os_return;
    }

    s_adc_is_running = true;

    for (;;)
    {
        os_sleep(ADC_PERIOD_MS); // 20ms 50hz
        // ADC1
        for (int i = 0; i < ADC1_NUM; i++)
        {
            *(adc1_ptr[i]) = lp_filter(g_filter1_group[i], adc1_buff[i] / 65535.0f);
        }

        // ADC3
        for (int i = 0; i < ADC3_NUM; i++)
        {
            *(adc3_ptr[i]) = lp_filter(g_filter3_group[i], adc3_buff[i] / 65535.0f);
        }
    }
}
void adc_bsp_init(void)
{
    os_task_create(adc_init_entry, "adc_init", NULL, OS_PRIORITY_LOWEST, OS_TASK_STACK_MIN + 1024);
    os_task_create(adc_entry, "adc", NULL, OS_PRIORITY_REALTIME, OS_TASK_STACK_MIN + 1024);
}

bool adc_is_running(void)
{
    return s_adc_is_running;
}

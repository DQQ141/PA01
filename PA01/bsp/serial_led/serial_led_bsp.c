#include "./serial_led_bsp.h"
// 此驱动不能使用其它内存，只能用axi
#undef MALLOC
#undef FREE
#define MALLOC(_size) malloc(_size)

// 驱动加载
#include "./../../lib/serial_led/serial_led.c"

// 端口加载
#include "./port/pwm.cc"

// SERIAL_LED组
sled_t *const g_serial_led_group[] = {
    &g_serial_led,
};
const uint32_t SERIAL_LED_GROUP_SIZE = countof(g_serial_led_group);

void serial_led_bsp_init(void)
{
    for (int i = 0; i < SERIAL_LED_GROUP_SIZE; i++)
    {
        serial_led_init(g_serial_led_group[i]);
    }
}

#include "./../player_bsp.h"

static const uint16_t data_dada[] __section_qspi_rom = {
    // PS 按键音
    2047, 2500, 3000, 3500, 2500, 2047, 1600, 1200, 800, 500, 2047, 2800, 3600, 2047, 1500,
    1200, 2047, 2600, 3200, 2047, 1400, 1100, 2047, 2300, 2900, 2047, 1200, 900, 2047, 2100,
    //
};

// 按照新的数据生成按键音
voice_t g_voice_dada = {
    .name = "dada",
    .data = (void *)data_dada,
    .data_bits = 12,
    .length = countof(data_dada),
    .sample_rate = 8000,
};

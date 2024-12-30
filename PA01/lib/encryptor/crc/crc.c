#include "./crc.h"
#include <stdlib.h>

// 辅助函数：反转位
static uint32_t reflect(uint32_t data, uint8_t bit_count)
{
    uint32_t reflection = 0;
    for (uint8_t i = 0; i < bit_count; ++i)
    {
        if (data & 1)
        {
            reflection |= (1 << ((bit_count - 1) - i));
        }
        data >>= 1;
    }
    return reflection;
}

void crc_table_create(crc_t *crc)
{
    ASSERT(crc != NULL);
    ASSERT(crc->crc_width == 8 || crc->crc_width == 16 || crc->crc_width == 32);

    crc->table = (uint32_t *)MALLOC(256 * sizeof(uint32_t));
    ASSERT(crc->table != NULL);

    uint32_t topbit = (1 << (crc->crc_width - 1));
    for (uint16_t i = 0; i < 256; i++)
    {
        uint32_t remainder = i << (crc->crc_width - 8);
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (remainder & topbit)
            {
                remainder = (remainder << 1) ^ crc->polynomial;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
        crc->table[i] = remainder & ((1 << crc->crc_width) - 1);
    }
}

void crc_table_free(crc_t *crc)
{
    ASSERT(crc != NULL);
    FREE(crc->table);
    crc->table = NULL;
}

void crc_table_print(crc_t *crc)
{
    ASSERT(crc != NULL);
    ASSERT(crc->table != NULL);

    uint32_t table_size = 256;
    PRINT("const uint32_t crc_table[%u] = {", table_size);

    for (uint32_t i = 0; i < table_size; i++)
    {
        if (i % 8 == 0)
        {
            PRINT("\r\n    ");
        }
        PRINT("0x%08X", crc->table[i]);

        if (i != table_size - 1)
        {
            PRINT(", ");
        }
    }

    PRINT("\r\n};\r\n");
}

uint32_t crc_calculate(crc_t *crc, const uint8_t *data, uint16_t length)
{
    ASSERT(crc != NULL);
    ASSERT(data != NULL);

    uint32_t crc_value = crc->initial_value;

    for (uint16_t i = 0; i < length; i++)
    {
        uint8_t byte = data[i];
        if (crc->reflect_input)
        {
            byte = reflect(byte, 8);
        }
        uint8_t table_index = (crc_value >> (crc->crc_width - 8)) ^ byte;
        crc_value = ((crc_value << 8) & ((1 << crc->crc_width) - 1)) ^ crc->table[table_index];
    }

    if (crc->reflect_output)
    {
        crc_value = reflect(crc_value, crc->crc_width);
    }

    crc_value ^= crc->final_xor_value;
    return crc_value & ((1 << crc->crc_width) - 1);
}

uint32_t crc_update(crc_t *crc, uint32_t last_crc_value, const uint8_t *data, uint16_t length)
{
    ASSERT(crc != NULL);
    ASSERT(data != NULL);

    uint32_t crc_value = last_crc_value;

    for (uint16_t i = 0; i < length; i++)
    {
        uint8_t byte = data[i];
        if (crc->reflect_input)
        {
            byte = reflect(byte, 8);
        }
        uint8_t table_index = (crc_value >> (crc->crc_width - 8)) ^ byte;
        crc_value = ((crc_value << 8) & ((1 << crc->crc_width) - 1)) ^ crc->table[table_index];
    }

    if (crc->reflect_output)
    {
        crc_value = reflect(crc_value, crc->crc_width);
    }

    crc_value ^= crc->final_xor_value;
    return crc_value & ((1 << crc->crc_width) - 1);
}

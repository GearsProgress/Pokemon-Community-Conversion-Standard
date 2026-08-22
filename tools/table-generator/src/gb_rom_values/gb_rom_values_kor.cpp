#include "gb_rom_values/gb_rom_values.h"

const struct GB_ROM gb_rom_values_kor[] = {
    {
        // KOR_GREEN
    },
    {
        // KOR_RED
    },
    {
        // KOR_BLUE
    },
    {
        // KOR_YELLOW,
    },
    {// KOR_GOLD
        .language = KOR_ID,
        .version = GOLD_ID,
        .generation = 2,
        .box_data_size = 0x0,
        .wCurrentBoxNum = 0x0,
        .wBoxDataStart = 0x0},
    {// KOR_SILVER
        .language = KOR_ID,
        .version = SILVER_ID,
        .generation = 2,
        .box_data_size = 0x0,
        .wCurrentBoxNum = 0x0,
        .wBoxDataStart = 0x0},
    {
        // KOR_CRYSTAL},
    }
};

const u16 gb_rom_values_kor_size = static_cast<u16>(sizeof(gb_rom_values_kor) / sizeof(struct GB_ROM));
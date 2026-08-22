#include "gb_rom_values/gb_rom_values.h"

const struct GB_ROM gb_rom_values_jpn[] = {
    {// JPN_GREEN
        .language = JPN_ID,
        .version = GREEN_ID,
        .generation = 1,
        .box_data_size = 0x0,
        .wCurrentBoxNum = 0x0,
        .wBoxDataStart = 0x0},
    {// JPN_RED
        .language = JPN_ID,
        .version = RED_ID,
        .generation = 1,
        .box_data_size = 0x0,
        .wCurrentBoxNum = 0x0,
        .wBoxDataStart = 0x0},
    {// JPN_BLUE
        .language = JPN_ID,
        .version = BLUE_ID,
        .generation = 1,
        .box_data_size = 0x0,
        .wCurrentBoxNum = 0x0,
        .wBoxDataStart = 0x0},
    {// JPN_YELLOW
        .language = JPN_ID,
        .version = YELLOW_ID,
        .generation = 1,
        .box_data_size = 0x0,
        .wCurrentBoxNum = 0x0,
        .wBoxDataStart = 0x0},
    {// JPN_GOLD
        .language = JPN_ID,
        .version = GOLD_ID,
        .generation = 2,
        .box_data_size = 0x0,
        .wCurrentBoxNum = 0x0,
        .wBoxDataStart = 0x0},
    {// JPN_SILVER
        .language = JPN_ID,
        .version = SILVER_ID,
        .generation = 2,
        .box_data_size = 0x0,
        .wCurrentBoxNum = 0x0,
        .wBoxDataStart = 0x0},
    {// JPN_CRYSTAL
        .language = JPN_ID,
        .version = CRYSTAL_ID,
        .generation = 2,
        .box_data_size = 0x0,
        .wCurrentBoxNum = 0x0,
        .wBoxDataStart = 0x0},
};

const u16 gb_rom_values_jpn_size = static_cast<u16>(sizeof(gb_rom_values_jpn) / sizeof(struct GB_ROM));
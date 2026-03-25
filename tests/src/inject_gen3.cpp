#include "catch_amalgamated.hpp"
#include "Gen3SaveManager.h"
#include "Gen3SaveFileReader.h"
#include "Gen3Pokemon.h"

#include <cstdio>

static const byte g3_bulbasaur_data[] = {
    0x25, 0x37, 0x98, 0xCE, 0x8A, 0x1D, 0x59, 0x8C, 0xBC, 0xCF, 0xC6, 0xBC, 0xBB, 0xCD, 0xBB, 0xCF, 
    0xCC, 0xFF, 0x02, 0x02, 0xCA, 0xCA, 0xE3, 0xE6, 0xDB, 0xFF, 0xFF, 0x00, 0x4B, 0x88, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x87, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x21, 0x00, 0x2D, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x23, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x05, 0xA2, 0x48, 0xF6, 0x05, 0x29, 0x00, 0x00, 0x00, 0x00
};

static const int SINGLE_MON_BYTES_IN_BOX = 80;

TEST_CASE("Gen3 Pokémon injection test", "[unit][gen3][injection]")
{
    const u8 zeros[128 * 1024] = {0};
    u8 comparisonBuffer[32];
    PokemonTables table;

    Gen3Pokemon bulbasaur(&table);
    bulbasaur.loadData(g3_bulbasaur_data, false);

    SECTION("Injecting bulbasaur into box 1")
    {
        FILE *savFile = tmpfile();
        Gen3SaveFileReader saveReader(savFile);
        // first write zeros to the file to create a blank save file
        fwrite(zeros, 1, sizeof(zeros), savFile);
        fseek(savFile, 0, SEEK_SET);

        Gen3SaveManager saveManager(EMERALD, LANGUAGE_UNKNOWN, saveReader);

        saveManager.addPokemonToBox(1, bulbasaur);
        saveManager.finishSave();

        u32 section5Offset = 5 * 4096; // each section is 4096 bytes
        fseek(savFile, section5Offset + 4, SEEK_SET);
        fseek(savFile, 30 * 80, SEEK_CUR); // skip the box metadata

        size_t bytesRead = fread(comparisonBuffer, 1, sizeof(comparisonBuffer), savFile);
        REQUIRE(bytesRead > 0);
        REQUIRE(memcmp(comparisonBuffer, g3_bulbasaur_data, sizeof(comparisonBuffer)) == 0);

        fclose(savFile);
    }

    SECTION("The first slot of box 3 is occupied")
    {
        FILE *savFile = tmpfile();
        // first write zeros to the file to create a blank save file
        fwrite(zeros, 1, sizeof(zeros), savFile);
        fseek(savFile, 0, SEEK_SET);

        Gen3SaveFileReader saveReader(savFile);
        Gen3SaveManager saveManager(EMERALD, LANGUAGE_UNKNOWN, saveReader);

        u32 section6Offset = 6 * 4096; // each section is 4096 bytes
        u32 boxOffsetInSection = 836;
        fseek(savFile, section6Offset + boxOffsetInSection, SEEK_SET);

        // we write the section6Offset as the personalityValue of the first pokemon in the box.
        // that way we force our injection to happen in the second box slot.
        fwrite(&section6Offset, 1, sizeof(section6Offset), savFile);

        saveManager.addPokemonToBox(2, bulbasaur);
        saveManager.finishSave();

        fseek(savFile, section6Offset + boxOffsetInSection + SINGLE_MON_BYTES_IN_BOX, SEEK_SET);
        size_t bytesRead = fread(comparisonBuffer, 1, sizeof(comparisonBuffer), savFile);
        REQUIRE(bytesRead > 0);
        REQUIRE(memcmp(comparisonBuffer, g3_bulbasaur_data, sizeof(comparisonBuffer)) == 0);

        fclose(savFile);
    }
}

TEST_CASE("Gen3 Pokémon injection test", "[unit][gen3][injection][dex]")
{
    const u8 zeros[128 * 1024] = {0};
    u8 comparisonBuffer[32];
    PokemonTables table;

    Gen3Pokemon bulbasaur(&table);
    bulbasaur.loadData(g3_bulbasaur_data, false);

    FILE *savFile = tmpfile();
    // first write zeros to the file to create a blank save file
    fwrite(zeros, 1, sizeof(zeros), savFile);
    fseek(savFile, 0, SEEK_SET);

    Gen3SaveFileReader saveReader(savFile);
    Gen3SaveManager saveManager(EMERALD, LANGUAGE_UNKNOWN, saveReader);

    saveManager.addPokemonToBox(1, bulbasaur);
    saveManager.finishSave();

    // seek to Emeralds' owned flag (slot A)
    fseek(savFile, 0x0028, SEEK_SET);
    if(!fread(comparisonBuffer, 1, 1, savFile))
    {
        REQUIRE(false); // fail the test if we can't read the owned flags
    }
    REQUIRE((comparisonBuffer[0] & 0x01) != 0); // bulbasaur is owned, so the first bit should be set

    // Seek to Emeralds' seen flags A (slot A)
    fseek(savFile, 0x005C, SEEK_SET);
    if(!fread(comparisonBuffer, 1, 1, savFile))
    {
        REQUIRE(false); // fail the test if we can't read the seen flags
    }
    REQUIRE((comparisonBuffer[0] & 0x01) != 0); // bulbasaur is seen, so the first bit should be set

    // Seek to Emeralds' seen flags B (slot A)
    fseek(savFile, 0x1988, SEEK_SET);
    if(!fread(comparisonBuffer, 1, 1, savFile))
    {
        REQUIRE(false); // fail the test if we can't read the seen flags
    }
    REQUIRE((comparisonBuffer[0] & 0x01) != 0); // bulbasaur is seen, so the first bit should be set

    // Seek to Emeralds' seen flags C (slot A)
    fseek(savFile, 0x4CA4, SEEK_SET);
    if(!fread(comparisonBuffer, 1, 1, savFile))
    {
        REQUIRE(false); // fail the test if we can't read the seen flags
    }
    REQUIRE((comparisonBuffer[0] & 0x01) != 0); // bulbasaur is seen, so the first bit should be set

    fclose(savFile);
}

TEST_CASE("Gen3 Pokemon box deletion test", "[unit][gen3][deletion][box]")
{
    const u8 zeros[128 * 1024] = {0};
    u8 comparisonBuffer[32];

    FILE *savFile = tmpfile();
    // first write zeros to the file to create a blank save file
    fwrite(zeros, 1, sizeof(zeros), savFile);

    u32 section6Offset = 6 * 4096; // each section is 4096 bytes
    u32 boxOffsetInSection = 836;
    fseek(savFile, section6Offset + boxOffsetInSection, SEEK_SET);
    // skip the first box slot
    fseek(savFile, SINGLE_MON_BYTES_IN_BOX, SEEK_CUR);

    fwrite(g3_bulbasaur_data, 1, sizeof(comparisonBuffer), savFile);

    Gen3SaveFileReader saveReader(savFile);
    Gen3SaveManager saveManager(EMERALD, LANGUAGE_UNKNOWN, saveReader);
    saveManager.removePokemonAtBoxIndex(2, 1);
    saveManager.finishSave();

    // now validate
    fseek(savFile, section6Offset + boxOffsetInSection + SINGLE_MON_BYTES_IN_BOX, SEEK_SET);
    fread(comparisonBuffer, 1, sizeof(comparisonBuffer), savFile);

    REQUIRE(memcmp(comparisonBuffer, zeros, sizeof(comparisonBuffer)) == 0);

    fclose(savFile);
}

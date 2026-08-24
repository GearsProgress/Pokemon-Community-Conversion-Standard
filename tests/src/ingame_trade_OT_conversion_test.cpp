#include "catch_amalgamated.hpp"
#include "GBPokemon.h"
#include "Gen1Pokemon.h"
#include "Gen2Pokemon.h"
#include "Gen3Pokemon.h"

#include <unordered_map>

#define OT_LENGTH_INT 7
#define OT_LENGTH_JPN 5

// PK1 files taken from https://projectpokemon.org/home/files/file/2575-every-npc-in-game-trade-and-gift-pok%C3%A9mon/
static const uint8_t farfetchd_international[]  = {
  0x01, 0x40, 0xff, 0x40, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x02, 0x2d, 0x40, 0x1c, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x07,
  0x23, 0x0f, 0x00, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x08, 0x00, 0x07, 0x00, 0x07, 0x00, 0x07, 0x5d,
  0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x83, 0x94, 0x97, 0x50, 0x50, 0x50,
  0x50, 0x50, 0x50, 0x50, 0x50
};

static const byte farfetchd_jpn[]  = {
  0x01, 0x40, 0xff, 0x40, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x02, 0x2d, 0x40, 0x1c, 0x00, 0x00, 0x3a,
  0x2c, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xda, 0x1d,
  0x23, 0x0f, 0x00, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x08, 0x00, 0x07, 0x00, 0x07, 0x00, 0x07, 0x5d,
  0x50, 0xe4, 0x61, 0xee, 0x61, 0xb5, 0xbc, 0xe2, 0xb3, 0x50, 0x50
};

TEST_CASE("In-game trade OT conversion test - map 0x5D 0x50 to predefined string", "[unit][gen1][ingame][conversion]")
{
    PokemonTables table;
    Gen1Pokemon farfetchd(&table);
    Gen3Pokemon converted(&table);
    char OTArray[16]; // utf-8 Japanese characters can take up to 3 bytes.

    const std::unordered_map<Language, const char*> langMap = {
      {JAPANESE, "トレーナー"},
      {ENGLISH, "TRAINER"},
      {FRENCH, "DRES."},
      {ITALIAN, "ALLEN."},
      {GERMAN, "TRAINER"},
      {SPANISH, "ENTREN."}
    };

    for(const auto& pair : langMap)
    {
      Language lang = pair.first;
      const char* expectedOT = pair.second;

      const uint8_t* data = (lang == JAPANESE) ? farfetchd_jpn : farfetchd_international;
      const size_t dataSize = (lang == JAPANESE) ? sizeof(farfetchd_jpn) : sizeof(farfetchd_international);
      const unsigned stringLength = (lang == JAPANESE) ? OT_LENGTH_JPN : OT_LENGTH_INT;

      farfetchd.loadData(lang, data, dataSize);
      farfetchd.convertToGen3(&converted, true);

      memset(OTArray, '\0', sizeof(OTArray));
      u8* curOTArray = (u8*)OTArray;
      for(unsigned i = 0; i < stringLength; ++i)
      {
        if(converted.getOTLetter(i) == POKEGBA_STRING_TERMINATOR)
        {
          break;
        }

        const u16 codepoint = table.gen3_charset[converted.getOTLetter(i)];
        const u32 numBytes = convert_utf16_to_utf8_char(codepoint, curOTArray);
        curOTArray += numBytes;
      }

      REQUIRE(converted.getSpeciesIndexNumber() == 83);
      REQUIRE(strcmp(OTArray, expectedOT) == 0);
    }
}
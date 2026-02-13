#include "catch_amalgamated.hpp"
#include "GBPokemon.h"
#include "Gen1Pokemon.h"
#include "Gen2Pokemon.h"
#include "Gen3Pokemon.h"

const byte charmander_data[] = {0xB0, 0x00, 0x16, 0x06, 0x00, 0x14, 0x14, 0x2D, 0x0A, 0x2D, 0x00, 0x00, 0x6F, 0xC6, 0x00, 0x00, 0xCD, 0x00, 0x2C, 0x00, 0x30, 0x00, 0x41, 0x00, 0x2B, 0x00, 0x32, 0x31, 0xD9, 0x23, 0x28, 0x00, 0x00};
const byte charmander_name[] = {0x80, 0x81, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const byte charmander_ot[] = {0x80, 0x81, 0x01, 0x02, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const byte charmander_id = 0xB0;

const byte cyndaquil_data[] = {0x9B, 0x00, 0x21, 0x2B, 0x6C, 0x00, 0x1D, 0x29, 0x00, 0x00, 0xCD, 0x00, 0x32, 0x00, 0x41, 0x00, 0x40, 0x00, 0x2B, 0x00, 0x2C, 0x59, 0x7B, 0x21, 0x1E, 0x14, 0x00, 0x4C, 0x00, 0x85, 0x01, 0x06};

TEST_CASE("Basic gen 1 test - check that pokemon data is parsed correctly", "[unit][gen1][pokemonparsing]")
{
    PokemonTables table;

    Gen1Pokemon charmander(&table);
    charmander.loadData(ENGLISH, charmander_data, charmander_name, charmander_ot, charmander_id);

    REQUIRE(charmander.getRawSpeciesIndexNumber() == 0xB0);
    REQUIRE(charmander.getSpeciesIndexNumber() == 4);
    REQUIRE(charmander.getCurrentHP() == 22);
    REQUIRE(charmander.getMove(0) == 10);
}

TEST_CASE("Basic gen 2 test - check that pokemon data is parsed correctly", "[unit][gen2][pokemonparsing]")
{
    PokemonTables table;

    Gen2Pokemon cyndaquil(&table);
    cyndaquil.loadData(ENGLISH, cyndaquil_data, charmander_name, charmander_ot, charmander_id);

    REQUIRE(cyndaquil.getSpeciesIndexNumber() == 0x9B);
    REQUIRE(cyndaquil.getMove(0) == 33);
    REQUIRE(cyndaquil.getLevel() == 6);
    // current HP should be 0, because it's a box pokémon. (32 bytes).
    // a party pokémon would have 48 bytes
    REQUIRE(cyndaquil.getCurrentHP() == 0);
}
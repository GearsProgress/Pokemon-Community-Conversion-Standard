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
const byte cyndaquil_id = 0x9B;

const byte g3_bulbasaur_data[] = {
    0x25, 0x37, 0x98, 0xCE, 0x8A, 0x1D, 0x59, 0x8C, 0xBC, 0xCF, 0xC6, 0xBC, 0xBB, 0xCD, 0xBB, 0xCF, 0xCC, 0xFF, 0x02, 0x02, 0xCA, 0xCA, 0xE3, 0xE6, 0xDB, 0xFF, 0xFF, 0x00, 0x4B, 0x88, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x87, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x21, 0x00, 0x2D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x05, 0xA2, 0x48, 0xF6, 0x05, 0x29, 0x00, 0x00, 0x00, 0x00, 0x14
};

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
    cyndaquil.loadData(ENGLISH, cyndaquil_data, charmander_name, charmander_ot, cyndaquil_id);

    REQUIRE(cyndaquil.getSpeciesIndexNumber() == 0x9B);
    REQUIRE(cyndaquil.getMove(0) == 33);
    REQUIRE(cyndaquil.getLevel() == 6);
    // current HP should be 0, because it's a box pokémon. (32 bytes).
    // a party pokémon would have 48 bytes
    REQUIRE(cyndaquil.getCurrentHP() == 0);
}

TEST_CASE("Basic gen2-gen3 conversion test", "[unit][gen2][gen3][conversion]")
{
    PokemonTables table;

    Gen2Pokemon cyndaquil(&table);
    cyndaquil.loadData(ENGLISH, cyndaquil_data, charmander_name, charmander_ot, cyndaquil_id);

    Gen3Pokemon convertedCyndaquil(&table);
    REQUIRE(cyndaquil.convertToGen3(&convertedCyndaquil, true) == true);

    REQUIRE(convertedCyndaquil.getTrainerID() == 7465);
    REQUIRE(convertedCyndaquil.getSecretID() == 0);
    REQUIRE(convertedCyndaquil.getLanguage() == 2);
    REQUIRE(convertedCyndaquil.getIsBadEgg() == false);
    REQUIRE(convertedCyndaquil.getHasSpecies() == true);
    REQUIRE(convertedCyndaquil.getUseEggName() == false);
    REQUIRE(convertedCyndaquil.getBlockBoxRS() == false);
    REQUIRE(convertedCyndaquil.getMarkings() == 0);
    REQUIRE(convertedCyndaquil.getSpeciesIndexNumber() == 0x9B);
    REQUIRE(convertedCyndaquil.getHeldItem() == 0);
    REQUIRE(convertedCyndaquil.getExpPoints() == 179);
    REQUIRE(convertedCyndaquil.getFriendship() == 70);
    REQUIRE(convertedCyndaquil.getEV(HP) == 0);
    REQUIRE(convertedCyndaquil.getEV(ATTACK) == 0);
    REQUIRE(convertedCyndaquil.getEV(DEFENSE) == 0);
    REQUIRE(convertedCyndaquil.getEV(SPECIAL_ATTACK) == 0);
    REQUIRE(convertedCyndaquil.getEV(SPECIAL_DEFENSE) == 0);
    REQUIRE(convertedCyndaquil.getEV(SPEED) == 0);
    REQUIRE(convertedCyndaquil.getContestCondition(COOLNESS) == 0);
    REQUIRE(convertedCyndaquil.getContestCondition(BEAUTY) == 0);
    REQUIRE(convertedCyndaquil.getContestCondition(CUTENESS) == 0);
    REQUIRE(convertedCyndaquil.getContestCondition(SMARTNESS) == 0);
    REQUIRE(convertedCyndaquil.getContestCondition(TOUGHNESS) == 0);
    REQUIRE(convertedCyndaquil.getSheen() == 0);
    REQUIRE(convertedCyndaquil.getMove(0) == 33); // TACKLE
    REQUIRE(convertedCyndaquil.getPPTotal(0) == 35);
    REQUIRE(convertedCyndaquil.getPPUpNum(0) == 0);
    REQUIRE(convertedCyndaquil.getMove(1) == 43); // LEER
    REQUIRE(convertedCyndaquil.getPPTotal(1) == 30);
    REQUIRE(convertedCyndaquil.getPPUpNum(1) == 0);
    REQUIRE(convertedCyndaquil.getMove(2) == 108); // SMOKESCREEN
    REQUIRE(convertedCyndaquil.getPPTotal(2) == 20);
    REQUIRE(convertedCyndaquil.getPPUpNum(2) == 0);
    REQUIRE(convertedCyndaquil.getMove(3) == 0);
    REQUIRE(convertedCyndaquil.getPPTotal(3) == 0);
    REQUIRE(convertedCyndaquil.getPPUpNum(3) == 0);
    REQUIRE(convertedCyndaquil.getPokerusStrain() == 0);
    REQUIRE(convertedCyndaquil.getPokerusDaysRemaining() == 0);
    REQUIRE(convertedCyndaquil.getMetLocation() == 255);
    REQUIRE(convertedCyndaquil.getLevelMet() == 6);
    REQUIRE(convertedCyndaquil.getGameOfOrigin() == 7);
    REQUIRE(convertedCyndaquil.getPokeballCaughtIn() == 4);
    REQUIRE(convertedCyndaquil.getOriginalTrainerGender() == 0);
    REQUIRE(convertedCyndaquil.getIsEgg() == false);
    REQUIRE(convertedCyndaquil.getAbility() == 0);
    REQUIRE(convertedCyndaquil.getFatefulEncounterObedience() == 0);
    REQUIRE(convertedCyndaquil.getIsShiny() == false);
}

TEST_CASE("Basic gen 3 test - check that pokemon data is parsed correctly", "[unit][gen3][pokemonparsing]")
{
    PokemonTables table;
    table.load_gen3_charset(ENGLISH);

    Gen3Pokemon bulbasaur(&table);
    bulbasaur.loadData(g3_bulbasaur_data, false);

    REQUIRE(bulbasaur.getTrainerID() == 7562);
    REQUIRE(bulbasaur.getSecretID() == 35929);
    REQUIRE(bulbasaur.getLanguage() == 2);
    REQUIRE(bulbasaur.getIsBadEgg() == false);
    REQUIRE(bulbasaur.getHasSpecies() == true);
    REQUIRE(bulbasaur.getUseEggName() == false);
    REQUIRE(bulbasaur.getBlockBoxRS() == false);
    REQUIRE(bulbasaur.getMarkings() == 0);
    //REQUIRE(bulbasaur.getChecksum() == 34891);
    REQUIRE(bulbasaur.getSpeciesIndexNumber() == 1);
    REQUIRE(bulbasaur.getHeldItem() == 0);
    REQUIRE(bulbasaur.getExpPoints() == 135);
    REQUIRE(bulbasaur.getFriendship() == 70);
    REQUIRE(bulbasaur.getEV(HP) == 0);
    REQUIRE(bulbasaur.getEV(ATTACK) == 0);
    REQUIRE(bulbasaur.getEV(DEFENSE) == 0);
    REQUIRE(bulbasaur.getEV(SPECIAL_ATTACK) == 0);
    REQUIRE(bulbasaur.getEV(SPECIAL_DEFENSE) == 0);
    REQUIRE(bulbasaur.getEV(SPEED) == 0);
    REQUIRE(bulbasaur.getIV(HP) == 8);
    REQUIRE(bulbasaur.getIV(ATTACK) == 18);
    REQUIRE(bulbasaur.getIV(DEFENSE) == 29);
    REQUIRE(bulbasaur.getIV(SPECIAL_ATTACK) == 16);
    REQUIRE(bulbasaur.getIV(SPECIAL_DEFENSE) == 20);
    REQUIRE(bulbasaur.getIV(SPEED) == 11);
    REQUIRE(bulbasaur.getContestCondition(COOLNESS) == 0);
    REQUIRE(bulbasaur.getContestCondition(BEAUTY) == 0);
    REQUIRE(bulbasaur.getContestCondition(CUTENESS) == 0);
    REQUIRE(bulbasaur.getContestCondition(SMARTNESS) == 0);
    REQUIRE(bulbasaur.getContestCondition(TOUGHNESS) == 0);
    REQUIRE(bulbasaur.getSheen() == 0);
    REQUIRE(bulbasaur.getMove(0) == 33); // TACKLE
    REQUIRE(bulbasaur.getPPTotal(0) == 35);
    REQUIRE(bulbasaur.getPPUpNum(0) == 0);
    REQUIRE(bulbasaur.getMove(1) == 45); // GROWL
    REQUIRE(bulbasaur.getPPTotal(1) == 40);
    REQUIRE(bulbasaur.getPPUpNum(1) == 0);
    REQUIRE(bulbasaur.getMove(2) == 0);
    REQUIRE(bulbasaur.getPPTotal(2) == 0);
    REQUIRE(bulbasaur.getPPUpNum(2) == 0);
    REQUIRE(bulbasaur.getMove(3) == 0);
    REQUIRE(bulbasaur.getPPTotal(3) == 0);
    REQUIRE(bulbasaur.getPPUpNum(3) == 0);
    REQUIRE(bulbasaur.getPokerusStrain() == 0);
    REQUIRE(bulbasaur.getPokerusDaysRemaining() == 0);
    REQUIRE(bulbasaur.getMetLocation() == 88);
    REQUIRE(bulbasaur.getLevelMet() == 5);
    REQUIRE(bulbasaur.getGameOfOrigin() == 4);
    REQUIRE(bulbasaur.getPokeballCaughtIn() == 4);
    REQUIRE(bulbasaur.getOriginalTrainerGender() == 1);
    REQUIRE(bulbasaur.getIsEgg() == false);
    REQUIRE(bulbasaur.getAbility() == 0);
    REQUIRE(bulbasaur.getFatefulEncounterObedience() == 0);
    REQUIRE(bulbasaur.getIsShiny() == false);
}
#include "catch_amalgamated.hpp"
#include "GBPokemon.h"
#include "Gen1Pokemon.h"
#include "Gen2Pokemon.h"
#include "Gen3Pokemon.h"

const byte charmander_data[] = {
    0xB0, 0x00, 0x16, 0x06, 0x00, 0x14, 0x14, 0x2D, 0x0A, 0x2D, 0x00, 0x00, 0x6F, 0xC6, 0x00, 0x00, 
    0xCD, 0x00, 0x2C, 0x00, 0x30, 0x00, 0x41, 0x00, 0x2B, 0x00, 0x32, 0x31, 0xD9, 0x23, 0x28, 0x00, 
    0x00
}
;
const byte charmander_name[] = {0x80, 0x81, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const byte charmander_ot[] = {0x80, 0x81, 0x01, 0x02, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const byte charmander_id = 0xB0;

const byte cyndaquil_data[] = {
    0x9B, 0x00, 0x21, 0x2B, 0x6C, 0x00, 0x1D, 0x29, 0x00, 0x00, 0xCD, 0x00, 0x32, 0x00, 0x41, 0x00, 
    0x40, 0x00, 0x2B, 0x00, 0x2C, 0x59, 0x7B, 0x21, 0x1E, 0x14, 0x00, 0x4C, 0x00, 0x85, 0x01, 0x06
};

const byte cyndaquil_id = 0x9B;

// Came from https://github.com/projectpokemon/In-GameSeries/blob/master/Gen%203/English/001%20-%20BULBASAUR%20-%20884BCE983725.pk3
const byte g3_bulbasaur_data[] = {
    0x25, 0x37, 0x98, 0xCE, 0x8A, 0x1D, 0x59, 0x8C, 0xBC, 0xCF, 0xC6, 0xBC, 0xBB, 0xCD, 0xBB, 0xCF, 
    0xCC, 0xFF, 0x02, 0x02, 0xCA, 0xCA, 0xE3, 0xE6, 0xDB, 0xFF, 0xFF, 0x00, 0x4B, 0x88, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x87, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x21, 0x00, 0x2D, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x23, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x05, 0xA2, 0x48, 0xF6, 0x05, 0x29, 0x00, 0x00, 0x00, 0x00
};

const byte g3_lapras_data_encrypted[] = {
    0x12, 0x90, 0x84, 0x26, 0x8A, 0x1D, 0x59, 0x8C, 0xC6, 0xBB, 0xCA, 0xCC, 0xBB, 0xCD, 0xFF, 0x08, 
    0x00, 0x73, 0x02, 0x02, 0xCA, 0xCA, 0xE3, 0xE6, 0xDB, 0xFF, 0xFF, 0x00, 0xB8, 0x4C, 0x00, 0x00, 
    0x1B, 0x8D, 0xDD, 0xAA, 0xD3, 0xC1, 0xDD, 0xAA, 0x98, 0xCB, 0xDD, 0xAA, 0x98, 0x8D, 0xDD, 0xAA, 
    0x98, 0x8D, 0xDD, 0xAA, 0x98, 0x8D, 0xDD, 0xAA, 0xAE, 0x8D, 0xFF, 0xAA, 0xF5, 0x8D, 0x1E, 0xAA, 
    0x86, 0x82, 0xD7, 0xAF, 0x98, 0x0B, 0xC4, 0x08, 0xD0, 0xDD, 0x04, 0x81, 0x98, 0x8D, 0xDD, 0xAA
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
    // PK3 files have a fixed GAEM substructure order.
    bulbasaur.loadData(g3_bulbasaur_data, false);

    REQUIRE(bulbasaur.getTrainerID() == 7562);
    REQUIRE(bulbasaur.getSecretID() == 35929);
    REQUIRE(bulbasaur.getLanguage() == 2);
    REQUIRE(bulbasaur.getIsBadEgg() == false);
    REQUIRE(bulbasaur.getHasSpecies() == true);
    REQUIRE(bulbasaur.getUseEggName() == false);
    REQUIRE(bulbasaur.getBlockBoxRS() == false);
    REQUIRE(bulbasaur.getMarkings() == 0);
    REQUIRE(bulbasaur.getChecksum() == 34891);
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

TEST_CASE("Basic gen 3 test - decryption", "[unit][gen3][pokemonparsing][decryption]")
{
    PokemonTables table;
    table.load_gen3_charset(ENGLISH);

    Gen3Pokemon lapras(&table);
    lapras.loadData(g3_lapras_data_encrypted);
    lapras.decryptSubstructures();
    
    REQUIRE(lapras.getPersonalityValue() == 0x26849012);
    REQUIRE(lapras.getTrainerID() == 7562);
    REQUIRE(lapras.getSecretID() == 35929);
    REQUIRE(lapras.getLanguage() == 2);
    REQUIRE(lapras.getIsBadEgg() == false);
    REQUIRE(lapras.getHasSpecies() == true);
    REQUIRE(lapras.getUseEggName() == false);
    REQUIRE(lapras.getBlockBoxRS() == false);
    REQUIRE(lapras.getMarkings() == 0);
    REQUIRE(lapras.getChecksum() == 19640);
    REQUIRE(lapras.getSpeciesIndexNumber() == 131);
    REQUIRE(lapras.getHeldItem() == 0);
    REQUIRE(lapras.getExpPoints() == 19531);
    REQUIRE(lapras.getFriendship() == 70);
    REQUIRE(lapras.getEV(HP) == 0);
    REQUIRE(lapras.getEV(ATTACK) == 0);
    REQUIRE(lapras.getEV(DEFENSE) == 0);
    REQUIRE(lapras.getEV(SPECIAL_ATTACK) == 0);
    REQUIRE(lapras.getEV(SPECIAL_DEFENSE) == 0);
    REQUIRE(lapras.getEV(SPEED) == 0);
    REQUIRE(lapras.getIV(HP) == 8);
    REQUIRE(lapras.getIV(ATTACK) == 2);
    REQUIRE(lapras.getIV(DEFENSE) == 20);
    REQUIRE(lapras.getIV(SPECIAL_ATTACK) == 29);
    REQUIRE(lapras.getIV(SPECIAL_DEFENSE) == 21);
    REQUIRE(lapras.getIV(SPEED) == 18);
    REQUIRE(lapras.getContestCondition(COOLNESS) == 0);
    REQUIRE(lapras.getContestCondition(BEAUTY) == 0);
    REQUIRE(lapras.getContestCondition(CUTENESS) == 0);
    REQUIRE(lapras.getContestCondition(SMARTNESS) == 0);
    REQUIRE(lapras.getContestCondition(TOUGHNESS) == 0);
    REQUIRE(lapras.getSheen() == 0);
    REQUIRE(lapras.getMove(0) == 54); // MIST
    REQUIRE(lapras.getPPTotal(0) == 30);
    REQUIRE(lapras.getPPUpNum(0) == 0);
    REQUIRE(lapras.getMove(1) == 34); // BODY SLAM
    REQUIRE(lapras.getPPTotal(1) == 15);
    REQUIRE(lapras.getPPUpNum(1) == 0);
    REQUIRE(lapras.getMove(2) == 109); // CONFUSE RAY
    REQUIRE(lapras.getPPTotal(2) == 10);
    REQUIRE(lapras.getPPUpNum(2) == 0);
    REQUIRE(lapras.getMove(3) == 195); // PERISH SONG
    REQUIRE(lapras.getPPTotal(3) == 5);
    REQUIRE(lapras.getPPUpNum(3) == 0);
    REQUIRE(lapras.getPokerusStrain() == 0);
    REQUIRE(lapras.getPokerusDaysRemaining() == 0);
    REQUIRE(lapras.getMetLocation() == 134);
    REQUIRE(lapras.getLevelMet() == 25);
    REQUIRE(lapras.getGameOfOrigin() == 4);
    REQUIRE(lapras.getPokeballCaughtIn() == 4);
    REQUIRE(lapras.getOriginalTrainerGender() == 1);
    REQUIRE(lapras.getIsEgg() == false);
    REQUIRE(lapras.getAbility() == 0);
    REQUIRE(lapras.getFatefulEncounterObedience() == 0);
    REQUIRE(lapras.getIsShiny() == false);
}

TEST_CASE("Basic gen 3 test - check checksum calculation", "[unit][gen3][checksum]")
{
    PokemonTables table;
    table.load_gen3_charset(ENGLISH);

    Gen3Pokemon bulbasaur(&table);
    // PK3 files have a fixed GAEM substructure order.
    bulbasaur.loadData(g3_bulbasaur_data, false);

    bulbasaur.updateChecksum();
    REQUIRE(bulbasaur.getChecksum() == 34891);
}
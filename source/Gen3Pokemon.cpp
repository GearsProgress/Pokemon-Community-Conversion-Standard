#include "Gen3Pokemon.h"
#include "pccs_utils.h"
#include <cstring>

using namespace PCCSUtils;

// determines the substructure offsets based on the personality value, and sets the substructOffsets array accordingly.
// This array will contain the index offsets for the substructs, in the order of G, A, E, M
static void determineSubstructOffsets(u32 substructLehmerCode, u8 *substructOffsets)
{
    // This array will contain the order of the substruct indices. (0=G, 1=A, 2=E, 3=M)
    u8 newIndexOrder[4];
    extractLehmerCode4(substructLehmerCode, newIndexOrder);

    for(u8 i=0; i < 4; ++i)
    {
        // so, we kinda need to reverse things: newIndexOrder tells us on every position which substruct should be there.
        // but substructOffsets maps a specific substruct to its offset in the specific GAEM order.
        substructOffsets[newIndexOrder[i]] = i;
    }
}

Gen3Pokemon::Gen3Pokemon(PokemonTables *table)
{
    pokeTable = table;
    dataArrayPtr = dataArray;
    dataArraySize = 80;
    nicknameArrayPtr = &dataArray[0x8];
    OTArrayPtr = &dataArray[0x14];
    isBigEndian = false;
    generation = 3;
    currSubstructureLehmerCode = 0;
};

Gen3Pokemon::Gen3Pokemon(const Gen3Pokemon &other)
    : Gen3Pokemon(other.pokeTable)
{
    currSubstructureLehmerCode = other.currSubstructureLehmerCode;
    memcpy(substructOffsets, other.substructOffsets, sizeof(substructOffsets));
    memcpy(dataArray, other.dataArray, sizeof(dataArray));
}

bool Gen3Pokemon::convertToGen3(Gen3Pokemon *g3p)
{
    return false;
}

// This is used to easily print out a Pokemon, when using a standard C++ terminal
#if ON_GBA
#else
void Gen3Pokemon::print(std::ostream &os)
{
    updateChecksum();
    updateSubstructureOrder(true);

    pokeTable->load_gen3_charset(ENGLISH);
    // if (!isValid)
    // {
    //     os << "ERROR: POKEMON IS INVALID\n";
    // }
    // else
    {
        os
            << "Personality Value: " << std::hex << getPersonalityValue() << std::dec
            << "\n\tLetter: " << (int)getUnownLetter()
            << "\n\tNature: " << getNature()
            << "\n\tGender: " << getGender() << '\n'
            << "Trainer ID: " << getTrainerID() << "\n"
            << "Secret ID: " << getSecretID() << "\n"
            << "Nickname: [";

        for (int i = 0; i < 10; i++)
        {
            os << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)getNicknameLetter(i) << (i < 9 ? ", " : "");
        }

        os << "] (";

        for (int i = 0; i < 10; i++)
        {
            os << (char)(pokeTable->gen3_charset[(int)getNicknameLetter(i)]);
        }

        os << ")" << "\n"
           << "Original Trainer: [";

        for (int i = 0; i < 7; i++)
        {
            os << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)getOTLetter(i) << (i < 6 ? ", " : "");
        }

        os << "] (";

        for (int i = 0; i < 7; i++)
        {
            os << (char)(pokeTable->gen3_charset[(int)getOTLetter(i)]);
        }

        os << ")" << "\n"
           << std::dec
           << "Language: " << getLanguage() << "\n"
           << "Is Bad Egg: " << getIsBadEgg() << "\n"
           << "Has Species: " << getHasSpecies() << "\n"
           << "Use Egg Name: " << getUseEggName() << "\n"
           << "Block Box RS: " << getBlockBoxRS() << "\n"
           << "Markings: " << getMarkings() << "\n"
           << "Checksum: " << getChecksum() << "\n"
           << "Species Index Number: " << getSpeciesIndexNumber() << "\n"
           << "Held Item: " << getHeldItem() << "\n"
           << "Experience: " << getExpPoints() << "\n"
           << "Friendship: " << getFriendship() << "\n"
           << "Stats: "
           << "\n\tHP: " << getEV(HP) << " EVs, " << getIV(HP) << " IVs"
           << "\n\tAttack: " << getEV(ATTACK) << " EVs, " << getIV(ATTACK) << " IVs"
           << "\n\tDefense: " << getEV(DEFENSE) << " EVs, " << getIV(DEFENSE) << " IVs"
           << "\n\tSpecial Attack: " << getEV(SPECIAL_ATTACK) << " EVs, " << getIV(SPECIAL_ATTACK) << " IVs"
           << "\n\tSpecial Defense: " << getEV(SPECIAL_DEFENSE) << " EVs, " << getIV(SPECIAL_DEFENSE) << " IVs"
           << "\n\tSpeed: " << getEV(SPEED) << " EVs, " << getIV(SPEED) << " IVs" << "\n"
           << "Contest Stats: "
           << "\n\tCoolness: " << getContestCondition(COOLNESS)
           << "\n\tBeauty: " << getContestCondition(BEAUTY)
           << "\n\tCuteness: " << getContestCondition(CUTENESS)
           << "\n\tSmartness: " << getContestCondition(SMARTNESS)
           << "\n\tToughness: " << getContestCondition(TOUGHNESS)
           << "\n\tSheen: " << getSheen() << "\n"
           << "Moves: "
           << "\n\t" << getMove(0) << " (" << getPPTotal(0) << " PP, " << getPPUpNum(0) << " PP Ups" << ")"
           << "\n\t" << getMove(1) << " (" << getPPTotal(1) << " PP, " << getPPUpNum(1) << " PP Ups" << ")"
           << "\n\t" << getMove(2) << " (" << getPPTotal(2) << " PP, " << getPPUpNum(2) << " PP Ups" << ")"
           << "\n\t" << getMove(3) << " (" << getPPTotal(3) << " PP, " << getPPUpNum(3) << " PP Ups" << ")" << "\n"
           << "Pokerus: "
           << "\n\tStrain: " << getPokerusStrain()
           << "\n\tDays Remaining: " << getPokerusDaysRemaining() << "\n"
           << "Met Location: " << getMetLocation() << "\n"
           << "Level Met: " << getLevelMet() << "\n"
           << "Game of Origin: " << getGameOfOrigin() << "\n"
           << "Pokeball Caught In: " << getPokeballCaughtIn() << "\n"
           << "Original Trainer Gender: " << getOriginalTrainerGender() << "\n"
           << "Is Egg: " << getIsEgg() << "\n"
           << "Ability: " << getAbility() << "\n"
           << "Fateful Encounter/Obedience: " << getFatefulEncounterObedience() << "\n"
           << "Is Shiny: " << getIsShiny() << "\n"
           << "\n"
           << "Substructure Perm: " << currSubstructureLehmerCode << "\n"
           << "Encryption Key: " << std::hex << ((getTrainerID() | getSecretID() << 16) ^ getPersonalityValue()) << std::dec << "\n"
           << "Substructure offsets:"
           << "\n\tG: " << getSubstructOffset(SUB_G)
           << "\n\tA: " << getSubstructOffset(SUB_A)
           << "\n\tE: " << getSubstructOffset(SUB_E)
           << "\n\tM: " << getSubstructOffset(SUB_M)
           << "\n";
    }
};
std::string Gen3Pokemon::printDataArray(bool encryptedData)
{
    updateSubstructureOrder(true);
    updateChecksum();
    if(encryptedData)
    {
        encryptSubstructures();
    }
    std::stringstream ss;
    for (int i = 0; i < 80; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)dataArray[i] << (i < 79 ? " " : "");
    }
    return ss.str();
}
#endif

// Get Nickname is different
// u32 getNickname() {};
// u32 getOriginalTrainerName() {}

u32 Gen3Pokemon::getNextRand_u32()
{
    currRand = (0x41C64E6D * currRand) + 0x6073;
    // std::cout << "Rolled: " << std::hex << currRand << "\n";
    return currRand;
}

u32 Gen3Pokemon::getPrevRand_u32()
{
    currRand = (currRand - 24691) * 4005161829;
    return currRand;
}

u16 Gen3Pokemon::getNextRand_u16()
{
    return getNextRand_u32() >> 16;
}

u16 Gen3Pokemon::getPrevRand_u16()
{
    return getPrevRand_u32() >> 16;
}

bool Gen3Pokemon::setPersonalityValue(u32 newVal) // Setting the Personality Value should modify some other values as well
{
    bool successful = setVar(personalityValue, newVal);
    if (successful)
    {
        //successful &= setAbility(getPersonalityValue() & 0b1);
    }
    return successful;
}

bool Gen3Pokemon::setAbility(u32 newVal) // We need to check if they have two abilities
{
    if (pokeTable->get_num_abilities(getSpeciesIndexNumber()) == 0)
    {
        newVal = 0;
    }
    internalAbility = newVal;
    return setVar(ability, getSubstructOffset(SUB_M), newVal);
}

// This is used to load our data in from an array and mark it as encrypted
void Gen3Pokemon::loadData(const byte incomingArray[], bool areSubstructsShuffled)
{
    memcpy(dataArrayPtr, incomingArray, dataArraySize);
    // reset currSubstructureLehmerCode before calling updateSubstructureOrder
    currSubstructureLehmerCode = areSubstructsShuffled ? 0xFFFFFFFF : 0;
    updateSubstructureOrder(!areSubstructsShuffled);
}

// And then some general functions
void Gen3Pokemon::decryptSubstructures()
{
    if (isEncrypted())
    {
        cryptStructures();
    }
};

void Gen3Pokemon::encryptSubstructures()
{
    if (!isEncrypted())
    {
        cryptStructures();
    }
};

void Gen3Pokemon::updateChecksum()
{
    const bool encryptionState = isEncrypted();
    decryptSubstructures();
    const u32 checksum = calculateChecksum();
    setChecksum(checksum);

    // re-encrypt if it was originally encrypted, since we don't want to mess with the encryption state of the data.
    if (encryptionState)
    {
        encryptSubstructures();
    }
}

void Gen3Pokemon::updateSubstructureOrder(bool shouldMove)
{
    u8 newSubstructOffsets[4];
    const u32 structureVal = getPersonalityValue() % 24;

    if(structureVal == currSubstructureLehmerCode)
    {
        return;
    }

    currSubstructureLehmerCode = structureVal;
    determineSubstructOffsets(structureVal, newSubstructOffsets);

    if(shouldMove)
    {
        u8 tempBuffer[48];
        uintptr_t oldOffset;
        uintptr_t newOffset;
        u8 *dataSectionStartPtr = dataArrayPtr + GEN3_PKMN_DATA_SUBSTRUCT_OFFSET;
        u32 i;

        // first we copy the old data sections into a temporary buffer, since they might get overwritten during the move process.
        memcpy(tempBuffer, dataSectionStartPtr, 48);

        // now we copy the data from the temporary buffer to the correct new locations in the data array, based on the new substructure offsets.
        // for each of the substructures (G, A, E, M), we find where it is currently located in the data array using substructOffsets, and then 
        // we copy it to its new location based on newSubstructOffsets.
        for(i=0; i < 4; ++i)
        {
            oldOffset = substructOffsets[i] * GEN3_POKEMON_SUBSTRUCTURE_SIZE;
            newOffset = newSubstructOffsets[i] * GEN3_POKEMON_SUBSTRUCTURE_SIZE;
            memcpy(dataSectionStartPtr + newOffset, tempBuffer + oldOffset, GEN3_POKEMON_SUBSTRUCTURE_SIZE);
        }
    }

    memcpy(substructOffsets, newSubstructOffsets, sizeof(newSubstructOffsets));
}

void Gen3Pokemon::updateSecurityData()
{
    updateSubstructureOrder(true);
    updateChecksum();
    encryptSubstructures();
}

byte Gen3Pokemon::getUnownLetter()
{
    if (getSpeciesIndexNumber() == 201)
    {

        u32 personalityValue = getPersonalityValue();
        return (
                   ((personalityValue & 0x03000000) >> 18) +
                   ((personalityValue & 0x00030000) >> 12) +
                   ((personalityValue & 0x00000300) >> 6) +
                   ((personalityValue & 0x00000003) >> 0)) %
               28;
    }
    else
    {
        return 255;
    }
};

Nature Gen3Pokemon::getNature()
{
    return (Nature)(getPersonalityValue() % 25);
};

Gender Gen3Pokemon::getGender()
{
    byte index = getSpeciesIndexNumber();
    u32 threshold = pokeTable->get_gender_threshold(index, true);

    switch (threshold)
    {
    case 0:
        return MALE;
    case 254:
        return FEMALE;
    case 255:
        return GENDERLESS;
    default:
        if ((getPersonalityValue() & 0xFF) >= threshold)
        {
            return MALE;
        }
        return FEMALE;
    }
}

int Gen3Pokemon::getAbilityFromPersonalityValue()
{
    if (internalAbility == 255)
    {
        return 255;
    }
    return getPersonalityValue() & 0b1;
}

int Gen3Pokemon::getSize()
{
    return 255;
}

bool Gen3Pokemon::getIsShiny()
{
    return (getTrainerID() ^
            getSecretID() ^
            (getPersonalityValue() >> 0 & 0xFFFF) ^
            (getPersonalityValue() >> 16 & 0xFFFF)) < 8;
}

bool Gen3Pokemon::setNicknameArray(byte nameArr[], int nameArrSize)
{
    for (int i = 0; i < nameArrSize; i++)
    {
        setNicknameLetter(i, nameArr[i]);
    }
    return true;
}

bool Gen3Pokemon::setOTArray(byte otArr[], int otArrSize)
{
    for (int i = 0; i < otArrSize; i++)
    {
        setOTLetter(i, otArr[i]);
    }
    return true;
}

bool Gen3Pokemon::isEncrypted()
{
    const u16 checksum = calculateChecksum();

    // the checksum is calculated on the decrypted data substruct.
    // So if the checksum doesn't match, then the data must still be encrypted.
    return (getChecksum() != checksum);
}

u16 Gen3Pokemon::calculateChecksum()
{
    u8 *cur = dataArrayPtr + GEN3_PKMN_DATA_SUBSTRUCT_OFFSET;
    const u8* const end = cur + 48;
    u16 checksum = 0x0000;
    u16 curWord;
    while(cur < end)
    {
        cur = PCCSUtils::readUint16(cur, curWord, Endianness::LITTLE);
        checksum += curWord;
    }
    return checksum;
}

void Gen3Pokemon::cryptStructures()
{
    const u32 key = (getTrainerID() | getSecretID() << 16) ^ getPersonalityValue();
    u32 *cur = (u32*)(dataArrayPtr + GEN3_PKMN_DATA_SUBSTRUCT_OFFSET);
    const u32 * const end = cur + (48 / sizeof(u32));

    // This operation should be the same for any endianness, since the key is just a u32 and the data is just being treated as an array of bytes. So we can just do it as u32s for speed.
    // that means: on a little endian system, the key will be stored in little endian and when reading the data as u32s, it will also be read in little endian, 
    // so the bytes will line up correctly for the XOR operation. On a big endian system, the key will be stored in big endian and when reading the data as u32s, 
    // it will also be read in big endian, so the bytes will also line up correctly for the XOR operation.
    while(cur < end)
    {
        *cur ^= key;
        ++cur;
    }
}

#pragma region
// Since there is only the Pokemon parent class, we can directly define these directly
const DataVarInfo
    // All of the data info variables
    Gen3Pokemon::personalityValue =
        {0x00, 32, 0},
    Gen3Pokemon::trainerID =
        {0x04, 16, 0},
    Gen3Pokemon::secretID =
        {0x06, 16, 0},
    Gen3Pokemon::nicknameLetterOne =
        {0x08, 8, 0}, // This is silly. Change this.
    Gen3Pokemon::nicknameLetterTwo =
        {0x09, 8, 0},
    Gen3Pokemon::nicknameLetterThree =
        {0x0A, 8, 0},
    Gen3Pokemon::nicknameLetterFour =
        {0x0B, 8, 0},
    Gen3Pokemon::nicknameLetterFive =
        {0x0C, 8, 0},
    Gen3Pokemon::nicknameLetterSix =
        {0x0D, 8, 0},
    Gen3Pokemon::nicknameLetterSeven =
        {0x0E, 8, 0},
    Gen3Pokemon::nicknameLetterEight =
        {0x0F, 8, 0},
    Gen3Pokemon::nicknameLetterNine =
        {0x10, 8, 0},
    Gen3Pokemon::nicknameLetterTen =
        {0x11, 8, 0},
    Gen3Pokemon::language =
        {0x12, 8, 0},
    Gen3Pokemon::isBadEgg =
        {0x13, 1, 0},
    Gen3Pokemon::hasSpecies =
        {0x13, 1, 1},
    Gen3Pokemon::useEggName =
        {0x13, 1, 2},
    Gen3Pokemon::blockBoxRS =
        {0x13, 1, 3},
    // Gen3Pokemon::unusedFlags =
    //     {0x13, 4, 4},
    Gen3Pokemon::originalTrainerNameLetterOne =
        {0x14, 8, 0}, // This is also silly. Change this.
    Gen3Pokemon::originalTrainerNameLetterTwo =
        {0x15, 8, 0},
    Gen3Pokemon::originalTrainerNameLetterThree =
        {0x16, 8, 0},
    Gen3Pokemon::originalTrainerNameLetterFour =
        {0x17, 8, 0},
    Gen3Pokemon::originalTrainerNameLetterFive =
        {0x18, 8, 0},
    Gen3Pokemon::originalTrainerNameLetterSix =
        {0x19, 8, 0},
    Gen3Pokemon::originalTrainerNameLetterSeven =
        {0x1A, 8, 0},
    Gen3Pokemon::markings =
        {0x1B, 4, 0},
    Gen3Pokemon::checksum =
        {0x1C, 16, 0};
// Gen3Pokemon::unknown =
//     {0x1E, 16, 0};

const DataVarInfo
    *Gen3Pokemon::nickname[10] = {
        &Gen3Pokemon::nicknameLetterOne,
        &Gen3Pokemon::nicknameLetterTwo,
        &Gen3Pokemon::nicknameLetterThree,
        &Gen3Pokemon::nicknameLetterFour,
        &Gen3Pokemon::nicknameLetterFive,
        &Gen3Pokemon::nicknameLetterSix,
        &Gen3Pokemon::nicknameLetterSeven,
        &Gen3Pokemon::nicknameLetterEight,
        &Gen3Pokemon::nicknameLetterNine,
        &Gen3Pokemon::nicknameLetterTen,
},
    *Gen3Pokemon::originalTrainerName[7] = {
        &Gen3Pokemon::originalTrainerNameLetterOne,
        &Gen3Pokemon::originalTrainerNameLetterTwo,
        &Gen3Pokemon::originalTrainerNameLetterThree,
        &Gen3Pokemon::originalTrainerNameLetterFour,
        &Gen3Pokemon::originalTrainerNameLetterFive,
        &Gen3Pokemon::originalTrainerNameLetterSix,
        &Gen3Pokemon::originalTrainerNameLetterSeven,
};

// data section G
const DataVarInfo
    Gen3Pokemon::speciesIndexNumber =
        {0x20 + 0x00, 16, 0},
    Gen3Pokemon::heldItem =
        {0x20 + 0x02, 16, 0},
    Gen3Pokemon::expPoints =
        {0x20 + 0x04, 32, 0},
    Gen3Pokemon::ppUpNumMoveOne =
        {0x20 + 0x08, 2, 0},
    Gen3Pokemon::ppUpNumMoveTwo =
        {0x20 + 0x08, 2, 2},
    Gen3Pokemon::ppUpNumMoveThree =
        {0x20 + 0x08, 2, 4},
    Gen3Pokemon::ppUpNumMoveFour =
        {0x20 + 0x08, 2, 6},
    Gen3Pokemon::friendship =
        {0x20 + 0x09, 8, 0};
// Gen3Pokemon::unused =
//    {0x20 + 0x0A, 16, 0};
const DataVarInfo
    *Gen3Pokemon::ppUpNums[4] = {
        &Gen3Pokemon::ppUpNumMoveOne,
        &Gen3Pokemon::ppUpNumMoveTwo,
        &Gen3Pokemon::ppUpNumMoveThree,
        &Gen3Pokemon::ppUpNumMoveFour,
};

// data section A
const DataVarInfo
    Gen3Pokemon::moveOne =
        {0x20 + 0x00, 16, 0},
    Gen3Pokemon::moveTwo =
        {0x20 + 0x02, 16, 0},
    Gen3Pokemon::moveThree =
        {0x20 + 0x04, 16, 0},
    Gen3Pokemon::moveFour =
        {0x20 + 0x06, 16, 0},
    Gen3Pokemon::moveOnePP =
        {0x20 + 0x08, 8, 0},
    Gen3Pokemon::moveTwoPP =
        {0x20 + 0x09, 8, 0},
    Gen3Pokemon::moveThreePP =
        {0x20 + 0x0A, 8, 0},
    Gen3Pokemon::moveFourPP =
        {0x20 + 0x0B, 8, 0};
const DataVarInfo
    *Gen3Pokemon::moves[4] = {
        &Gen3Pokemon::moveOne,
        &Gen3Pokemon::moveTwo,
        &Gen3Pokemon::moveThree,
        &Gen3Pokemon::moveFour,
},
    *Gen3Pokemon::ppUpTotals[4] = {
        &Gen3Pokemon::moveOnePP,
        &Gen3Pokemon::moveTwoPP,
        &Gen3Pokemon::moveThreePP,
        &Gen3Pokemon::moveFourPP,
};

// data section E
const DataVarInfo
    Gen3Pokemon::hpEVs =
        {0x20 + 0x00, 8, 0},
    Gen3Pokemon::attackEVs =
        {0x20 + 0x01, 8, 0},
    Gen3Pokemon::defenseEVs =
        {0x20 + 0x02, 8, 0},
    Gen3Pokemon::speedEVs =
        {0x20 + 0x03, 8, 0},
    Gen3Pokemon::specialAttackEVs =
        {0x20 + 0x04, 8, 0},
    Gen3Pokemon::specialDefenseEVs =
        {0x20 + 0x05, 8, 0},
    Gen3Pokemon::coolnessCondition =
        {0x20 + 0x06, 8, 0},
    Gen3Pokemon::beautyCondition =
        {0x20 + 0x07, 8, 0},
    Gen3Pokemon::cutenessCondition =
        {0x20 + 0x08, 8, 0},
    Gen3Pokemon::smartnessCondition =
        {0x20 + 0x09, 8, 0},
    Gen3Pokemon::toughnessCondition =
        {0x20 + 0x0A, 8, 0},
    Gen3Pokemon::sheen =
        {0x20 + 0x0B, 8, 0};

const DataVarInfo
    *Gen3Pokemon::EVs[6] = {
        &Gen3Pokemon::hpEVs,
        &Gen3Pokemon::attackEVs,
        &Gen3Pokemon::defenseEVs,
        &Gen3Pokemon::speedEVs,
        &Gen3Pokemon::specialAttackEVs,
        &Gen3Pokemon::specialDefenseEVs,
},
    *Gen3Pokemon::contestConditions[5] = {
        &Gen3Pokemon::coolnessCondition,
        &Gen3Pokemon::beautyCondition,
        &Gen3Pokemon::cutenessCondition,
        &Gen3Pokemon::smartnessCondition,
        &Gen3Pokemon::toughnessCondition,
};

const DataVarInfo

    // data section M
    Gen3Pokemon::pokerusStrain =
        {0x20 + 0x00, 4, 0},
    Gen3Pokemon::pokerusDaysRemaining =
        {0x20 + 0x00, 4, 4},
    Gen3Pokemon::metLocation =
        {0x20 + 0x01, 8, 0},
    Gen3Pokemon::levelMet =
        {0x20 + 0x02, 7, 0},
    Gen3Pokemon::gameOfOrigin =
        {0x20 + 0x02, 4, 7},
    Gen3Pokemon::pokeballCaughtIn =
        {0x20 + 0x02, 4, 11},
    Gen3Pokemon::originalTrainerGender =
        {0x20 + 0x02, 1, 15},
    Gen3Pokemon::hpIVs =
        {0x20 + 0x04, 5, 0},
    Gen3Pokemon::attackIVs =
        {0x20 + 0x04, 5, 5},
    Gen3Pokemon::defenseIVs =
        {0x20 + 0x04, 5, 10},
    Gen3Pokemon::speedIVs =
        {0x20 + 0x04, 5, 15},
    Gen3Pokemon::specialAttackIVs =
        {0x20 + 0x04, 5, 20},
    Gen3Pokemon::specialDefenseIVs =
        {0x20 + 0x04, 5, 25},
    Gen3Pokemon::isEgg =
        {0x20 + 0x04, 1, 30},
    Gen3Pokemon::ability =
        {0x20 + 0x04, 1, 31},
    Gen3Pokemon::coolNormalContestRibbon =
        {0x20 + 0x08, 1, 0}, // This is also very silly. Change it.
    Gen3Pokemon::coolSuperContestRibbon =
        {0x20 + 0x08, 1, 0},
    Gen3Pokemon::coolHyperContestRibbon =
        {0x20 + 0x08, 1, 1},
    Gen3Pokemon::coolMasterContestRibbon =
        {0x20 + 0x08, 1, 2},
    Gen3Pokemon::beautyNormalContestRibbon =
        {0x20 + 0x08, 1, 3},
    Gen3Pokemon::beautySuperContestRibbon =
        {0x20 + 0x08, 1, 4},
    Gen3Pokemon::beautyHyperContestRibbon =
        {0x20 + 0x08, 1, 5},
    Gen3Pokemon::beautyMasterContestRibbon =
        {0x20 + 0x08, 1, 6},
    Gen3Pokemon::cuteNormalContestRibbon =
        {0x20 + 0x08, 1, 7},
    Gen3Pokemon::cuteSuperContestRibbon =
        {0x20 + 0x08, 1, 8},
    Gen3Pokemon::cuteHyperContestRibbon =
        {0x20 + 0x08, 1, 9},
    Gen3Pokemon::cuteMasterContestRibbon =
        {0x20 + 0x08, 1, 10},
    Gen3Pokemon::smartNormalContestRibbon =
        {0x20 + 0x08, 1, 11},
    Gen3Pokemon::smartSuperContestRibbon =
        {0x20 + 0x08, 1, 12},
    Gen3Pokemon::smartHyperContestRibbon =
        {0x20 + 0x08, 1, 13},
    Gen3Pokemon::smartMasterContestRibbon =
        {0x20 + 0x08, 1, 14},
    Gen3Pokemon::toughNormalContestRibbon =
        {0x20 + 0x08, 1, 15},
    Gen3Pokemon::toughSuperContestRibbon =
        {0x20 + 0x08, 1, 16},
    Gen3Pokemon::toughHyperContestRibbon =
        {0x20 + 0x08, 1, 17},
    Gen3Pokemon::toughMasterContestRibbon =
        {0x20 + 0x08, 1, 18},
    Gen3Pokemon::championRibbon =
        {0x20 + 0x08, 1, 19},
    Gen3Pokemon::winningRibbon =
        {0x20 + 0x08, 1, 20},
    Gen3Pokemon::victoryRibbon =
        {0x20 + 0x08, 1, 21},
    Gen3Pokemon::artistRibbon =
        {0x20 + 0x08, 1, 22},
    Gen3Pokemon::effortRibbon =
        {0x20 + 0x08, 1, 23},
    Gen3Pokemon::battleChampionRibbon =
        {0x20 + 0x08, 1, 24},
    Gen3Pokemon::regionalChampionRibbon =
        {0x20 + 0x08, 1, 25},
    Gen3Pokemon::nationalChampionRibbon =
        {0x20 + 0x08, 1, 26},
    Gen3Pokemon::countryRibbon =
        {0x20 + 0x08, 1, 27},
    Gen3Pokemon::nationalRibbon =
        {0x20 + 0x08, 1, 28},
    Gen3Pokemon::earthRibbon =
        {0x20 + 0x08, 1, 29},
    Gen3Pokemon::unusedRibbons =
        {0x20 + 0x08, 1, 30},
    Gen3Pokemon::fatefulEncounterObedience =
        {0x20 + 0x08, 1, 31};

const DataVarInfo
    *Gen3Pokemon::IVs[6] = {
        &Gen3Pokemon::hpIVs,
        &Gen3Pokemon::attackIVs,
        &Gen3Pokemon::defenseIVs,
        &Gen3Pokemon::speedIVs,
        &Gen3Pokemon::specialAttackIVs,
        &Gen3Pokemon::specialDefenseIVs,
},
    *Gen3Pokemon::ribbons[31] = {
        &Gen3Pokemon::coolNormalContestRibbon,
        &Gen3Pokemon::coolSuperContestRibbon,
        &Gen3Pokemon::coolHyperContestRibbon,
        &Gen3Pokemon::coolMasterContestRibbon,
        &Gen3Pokemon::beautyNormalContestRibbon,
        &Gen3Pokemon::beautySuperContestRibbon,
        &Gen3Pokemon::beautyHyperContestRibbon,
        &Gen3Pokemon::beautyMasterContestRibbon,
        &Gen3Pokemon::cuteNormalContestRibbon,
        &Gen3Pokemon::cuteSuperContestRibbon,
        &Gen3Pokemon::cuteHyperContestRibbon,
        &Gen3Pokemon::cuteMasterContestRibbon,
        &Gen3Pokemon::smartNormalContestRibbon,
        &Gen3Pokemon::smartSuperContestRibbon,
        &Gen3Pokemon::smartHyperContestRibbon,
        &Gen3Pokemon::smartMasterContestRibbon,
        &Gen3Pokemon::toughNormalContestRibbon,
        &Gen3Pokemon::toughSuperContestRibbon,
        &Gen3Pokemon::toughHyperContestRibbon,
        &Gen3Pokemon::toughMasterContestRibbon,
        &Gen3Pokemon::championRibbon,
        &Gen3Pokemon::winningRibbon,
        &Gen3Pokemon::victoryRibbon,
        &Gen3Pokemon::artistRibbon,
        &Gen3Pokemon::effortRibbon,
        &Gen3Pokemon::battleChampionRibbon,
        &Gen3Pokemon::regionalChampionRibbon,
        &Gen3Pokemon::nationalChampionRibbon,
        &Gen3Pokemon::countryRibbon,
        &Gen3Pokemon::nationalRibbon,
        &Gen3Pokemon::earthRibbon,
};
#pragma endregion
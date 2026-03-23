#include "Gen3SaveManager.h"
#include "IGen3SaveFileReader.h"
#include "Gen3Pokemon.h"

#include <cstring>

#define SAVE_A_OFFSET       0 // Offset of Game Save A
#define SAVE_B_OFFSET       0xE000 // Offset of Game Save B
#define SECTION_ID_OFFSET   0x0FF4
#define SECTION_CHECKSUM_OFFSET 0x0FF6
#define SECTION_SIZE         0x1000
#define SAVE_INDEX_OFFSET   0x0FFC
#define FIRST_BOX_SECTION_ID 5
#define NUM_BOX_SECTIONS 9
#define BOX_BYTES_PER_SECTION 3968
#define MONS_PER_BOX 30
#define SINGLE_MON_BYTES_IN_BOX 80

static const u16 ownedFlagsBaseOffset = 0x0028;

static void seekToSectionOffset(IGen3SaveFileReader& saveReader, const Gen3SaveMetadata& saveMetadata, u8 sectionId, u32 offsetWithinSection)
{
    saveReader.seek(saveMetadata.sectionMap_[sectionId] + offsetWithinSection);
}

static u16 calculateSectionChecksum(IGen3SaveFileReader& saveReader, u8 sectionId)
{
    unsigned num_bytes_to_checksum;
    u32 checksum = 0;
    u32 cur;

    switch(sectionId)
    {
    case 0:
        num_bytes_to_checksum = 3884;
        break;
    case 4:
        num_bytes_to_checksum = 3848;
        break;
    case 13:
        num_bytes_to_checksum = 2000;
        break;
    default:
        num_bytes_to_checksum = 3968;
        break;
    }

    // https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_III)#Checksum
    for(unsigned i = 0; i < num_bytes_to_checksum; i += sizeof(u32))
    {
        saveReader.readUint32(cur, Endianness::LITTLE);
        checksum += cur;
    }
    const u16 reduced_checksum = ((checksum & 0xFFFF0000) >> 16) + (checksum & 0x0000FFFF);

    // rewind back to where we started.
    saveReader.rewind(num_bytes_to_checksum);

    return reduced_checksum;
}

// validates the current section checksum.
// Note: the caller needs to seek to the start of the section.
static bool validateSectionChecksum(IGen3SaveFileReader& saveReader, u8 sectionId)
{
    u16 stored_checksum, calculated_checksum;

    saveReader.seek(SECTION_CHECKSUM_OFFSET);
    saveReader.readUint16(stored_checksum, Endianness::LITTLE);
    
    // return to the start of the section
    const long rewindAmount = -static_cast<long>(SECTION_CHECKSUM_OFFSET + sizeof(u16));
    saveReader.rewind(rewindAmount);

    calculated_checksum = calculateSectionChecksum(saveReader, sectionId);
    return (stored_checksum == calculated_checksum);
}

static void updateSectionChecksum(IGen3SaveFileReader& saveReader, const Gen3SaveMetadata& saveMetadata, u8 sectionId)
{
    seekToSectionOffset(saveReader, saveMetadata, sectionId, 0);
    u16 new_checksum = calculateSectionChecksum(saveReader, sectionId);

    seekToSectionOffset(saveReader, saveMetadata, sectionId, SECTION_CHECKSUM_OFFSET);
    saveReader.writeUint16(new_checksum, Endianness::LITTLE);
}

static void getNationalDexUnlockFieldOffsets(Game game, u32& offsetFieldA, u32& offsetFieldB, u32& offsetFieldC)
{
    // https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_III)#Pok%C3%A9dex_data
    switch(game)
    {
    case Game::RUBY:
    case Game::SAPPHIRE:
        offsetFieldA = 0x0019;
        offsetFieldB = 0x03A6;
        offsetFieldC = 0x044C;
        break;
    case Game::EMERALD:
        offsetFieldA = 0x0019;
        offsetFieldB = 0x0402;
        offsetFieldC = 0x04A8;
        break;
    case Game::FIRERED:
    case Game::LEAFGREEN:
        offsetFieldA = 0x001B;
        offsetFieldB = 0x0068;
        offsetFieldC = 0x011C;
        break;
    default:
        offsetFieldA = 0;
        offsetFieldB = 0;
        offsetFieldC = 0;
        break;
    }
}

static void getSeenFlagOffsetsForGame(Game game, u32& offsetFieldA, u32& offsetFieldB, u32& offsetFieldC)
{
    offsetFieldA = 0x005C;

    switch(game)
    {
    case Game::RUBY:
    case Game::SAPPHIRE:
        offsetFieldB = 0x0938;
        offsetFieldC = 0x0C0C;
        break;
    case Game::EMERALD:
        offsetFieldB = 0x0988;
        offsetFieldC = 0x0CA4;
        break;
    case Game::FIRERED:
    case Game::LEAFGREEN:
        offsetFieldB = 0x05F8;
        offsetFieldC = 0x0B98;
        break;
    default:
        offsetFieldB = 0;
        offsetFieldC = 0;
        break;
    }
}

static bool getBitFlag(IGen3SaveFileReader& saveReader, const Gen3SaveMetadata& saveMetadata, u8 sectionId, u32 flagBaseOffset, u32 flagIndex)
{
    u8 flagByte;

    seekToSectionOffset(saveReader, saveMetadata, sectionId, flagBaseOffset + (flagIndex / 8));
    saveReader.readUint8(flagByte);

    return (flagByte >> (flagIndex & 7)) & 1;
}

static void setBitFlag(IGen3SaveFileReader& saveReader, Gen3SaveMetadata& saveMetadata, u8 sectionId, u32 flagBaseOffset, u32 flagIndex, bool enabled)
{
    u8 flagByte;

    seekToSectionOffset(saveReader, saveMetadata, sectionId, flagBaseOffset + (flagIndex / 8));
    saveReader.readUint8(flagByte);
    saveReader.rewind(1);

    if(enabled)
    {
        flagByte |= (1 << (flagIndex & 7));
    }
    else
    {
        flagByte &= ~(1 << (flagIndex & 7));
    }

    saveReader.writeUint8(flagByte);

    saveMetadata.sectionModified_[sectionId] = true;
}

/**
 * @brief This class exists to abstract the fact that
 * PC boxes are stored across multiple sections in the save file.
 * It will provide an interface to read and write bytes to the box 
 * without needing to worry about the underlying structure of the save file
 * 
 * But the underlying structure does look like a large buffer to store 
 * the pokémon for ALL boxes, that is split up near the section boundaries.
 * That does mean that a box can be split across two sections, and that's exactly
 * why a class like this can be useful, to abstract away that complexity and just let us read and write
 *  to the box as if it were a single contiguous buffer.
 */
class Gen3BoxBufferManager
{
public:
    Gen3BoxBufferManager(Gen3SaveMetadata& saveMetadata, IGen3SaveFileReader& saveReader)
        : saveMetadata_(saveMetadata)
        , saveReader_(saveReader)
    {
    }

    
    void seek(u32 offset_in_box_buffer)
    {
        const u32 curBoxFieldSize = 4;
        // we store the offset instead of doing the seek immediately.
        // After all, we should be wary of external code triggering some kind of seek on the file.
        curOffset_ = curBoxFieldSize + offset_in_box_buffer;

        const u16 sectionId = convertOffsetToSectionId(curOffset_);
        u32 offsetWithinSection = curOffset_ % BOX_BYTES_PER_SECTION;
        saveReader_.seek(saveMetadata_.sectionMap_[sectionId] + offsetWithinSection);
    }

    void rewind(u32 numBytes)
    {
        saveReader_.rewind(numBytes);
        curOffset_ -= numBytes;
    }

    void advance(u32 numBytes)
    {
        saveReader_.advance(numBytes);
        curOffset_ += numBytes;
    }

    void readUint32(u32& outDWord, Endianness fieldEndianness)
    {
        saveReader_.readUint32(outDWord, fieldEndianness);
        curOffset_ += sizeof(u32);
    }

    // Note: this function will trigger a seek
    void read(u8* outBuffer, u32 numBytes)
    {
        const u16 sectionId = convertOffsetToSectionId(curOffset_);
        u32 offsetWithinSection = curOffset_ % BOX_BYTES_PER_SECTION;
        saveReader_.seek(saveMetadata_.sectionMap_[sectionId] + offsetWithinSection);

        u32 bytesToRead = (numBytes > (BOX_BYTES_PER_SECTION - offsetWithinSection)) ? (BOX_BYTES_PER_SECTION - offsetWithinSection) : numBytes;
        saveReader_.read(outBuffer, bytesToRead);
        curOffset_ += bytesToRead;
        numBytes -= bytesToRead;

        if(numBytes > 0)
        {
            // we need to read from the next section
            // calling read recursively should do this for us, because the new offset ends up in the next section
            // and a seek will be triggered
            read(outBuffer + bytesToRead, numBytes);
        }
    }

    // Note: this function will trigger a seek
    void write(const u8* inBuffer, u32 numBytes)
    {
        const u16 sectionId = convertOffsetToSectionId(curOffset_);
        u32 offsetWithinSection = curOffset_ % BOX_BYTES_PER_SECTION;
        saveReader_.seek(saveMetadata_.sectionMap_[sectionId] + offsetWithinSection);

        u32 bytesToWrite = (numBytes > (BOX_BYTES_PER_SECTION - offsetWithinSection)) ? (BOX_BYTES_PER_SECTION - offsetWithinSection) : numBytes;
        saveReader_.write(inBuffer, bytesToWrite);
        curOffset_ += bytesToWrite;
        numBytes -= bytesToWrite;

        // mark the current section as modified.
        saveMetadata_.sectionModified_[sectionId] = true;

        if(numBytes > 0)
        {
            // we need to write to the next section
            // calling write recursively should do this for us, because the new offset ends up in the next section
            // and a seek will be triggered
            write(inBuffer + bytesToWrite, numBytes);
        }
    }
protected:
private:
    u16 convertOffsetToSectionId(u32 offset) const
    {
        // we know that the first box section starts at section id 5, 
        // and that each section can store BOX_BYTES_PER_SECTION bytes (except for the last one)
        return (offset / BOX_BYTES_PER_SECTION) + FIRST_BOX_SECTION_ID;;
    }

    Gen3SaveMetadata &saveMetadata_;
    IGen3SaveFileReader &saveReader_;
    u32 curOffset_;
};

/**
 * @brief This function picks the latest save slot from the save file,
 * taking empty/corrupted slots into account.
 */
static u32 pickSaveSlot(IGen3SaveFileReader& saveReader)
{
    u32 saveCountA, saveCountB;
    bool saveAValid, saveBValid;
    
    // deal with empty/corrupted slots by validating the checksum of the first section.
    saveAValid = validateSectionChecksum(saveReader, 0);
    saveReader.seek(SAVE_B_OFFSET);
    saveBValid = validateSectionChecksum(saveReader, 0);

    if(!saveAValid)
    {
        return SAVE_B_OFFSET;
    }
    else if(!saveBValid)
    {
        return SAVE_A_OFFSET;
    }

    // both are valid.
    // establish currentSaveoffset by comparing the save counts of both save slots
    saveReader.seek(SAVE_A_OFFSET + SAVE_INDEX_OFFSET);
    saveReader.readUint32(saveCountA, Endianness::LITTLE);
    saveReader.seek(SAVE_B_OFFSET + SAVE_INDEX_OFFSET);
    saveReader.readUint32(saveCountB, Endianness::LITTLE);

    return (saveCountA >= saveCountB) ? SAVE_A_OFFSET : SAVE_B_OFFSET;
}

/**
 * @brief This function searches through the save file to find the
 * current save slot and maps the sections within that save slot.
 */
static void indexSave(IGen3SaveFileReader& saveReader, Gen3SaveMetadata& saveMetadata)
{
    u16 sectionId;

    saveMetadata.currentSaveOffset_ = pickSaveSlot(saveReader);

    // now let's map the sections.
    // The sections within the save slot are rotated on every save. So the save doesn't 
    // start at the first section. However, the next sections do follow sequentially.
    // https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_III)#Section_ID
    saveReader.seek(saveMetadata.currentSaveOffset_ + SECTION_ID_OFFSET);
    saveReader.readUint16(sectionId, Endianness::LITTLE);
    for(unsigned i = 0; i < NUM_SAVE_SECTIONS; ++i)
    {
        saveMetadata.sectionMap_[sectionId] = saveMetadata.currentSaveOffset_ + (i * SECTION_SIZE);
        sectionId = (sectionId + 1) % NUM_SAVE_SECTIONS;
    }
    
    // initialize all sections as unmodified
    memset(saveMetadata.sectionModified_, 0, sizeof(saveMetadata.sectionModified_));

    // now establish the current PC box index
    seekToSectionOffset(saveReader, saveMetadata, FIRST_BOX_SECTION_ID, 0);
    saveReader.readUint32(saveMetadata.currentPcBoxIndex_, Endianness::LITTLE);
}

Gen3SaveManager::Gen3SaveManager(Game gameType, IGen3SaveFileReader &saveReader)
    : gameType_(gameType)
    , saveMetadata_()
    , saveReader_(saveReader)
{
    indexSave(saveReader_, saveMetadata_);
}

Gen3SaveManager::~Gen3SaveManager()
{
}

u32 Gen3SaveManager::getCurrentBoxIndex() const
{
    return saveMetadata_.currentPcBoxIndex_;
}

u32 Gen3SaveManager::addPokemonToBox(int boxIndex, Gen3Pokemon& pokemon)
{
    Gen3BoxBufferManager boxBufferManager(saveMetadata_, saveReader_);
    Gen3Pokemon decryptedCopy(pokemon);
    u32 personalityValue;

    // decrypt for species index
    decryptedCopy.decryptSubstructures();
    const u16 speciesIndex = decryptedCopy.getSpeciesIndexNumber();

    // make sure the pokemon is encrypted first
    pokemon.encryptSubstructures();

    // seek to the start of the box
    const u32 boxStartOffset = boxIndex * (MONS_PER_BOX * SINGLE_MON_BYTES_IN_BOX);
    boxBufferManager.seek(boxStartOffset);
    
    // find an empty slot in the box by looking for a pokemon with personality value of 0.
    for(unsigned i=0; i < MONS_PER_BOX; ++i)
    {
        // TODO: readUint32FromFile() needs a file seek first
        boxBufferManager.readUint32(personalityValue, Endianness::LITTLE);
        boxBufferManager.rewind(sizeof(u32));

        if(personalityValue == 0) // this slot is empty, we can write our pokemon here
        {
            boxBufferManager.write(pokemon.dataArrayPtr, SINGLE_MON_BYTES_IN_BOX); // write the pokemon data to the box
            setPokemonOwned(speciesIndex, true);
            setPokemonSeen(speciesIndex, true);
            return i; // return the index within the box where we added the pokemon
        }
        // note that the readUint32FromFile() call did not advance the internal offset
        // of boxBufferManager. So we can just seek immediately to the next pokemon slot
        // by advancing the offset by the size of a pokemon. 
        boxBufferManager.advance(SINGLE_MON_BYTES_IN_BOX);
    }
    return UINT32_MAX;
}

bool Gen3SaveManager::removePokemonAtBoxIndex(int boxIndex, unsigned pokemonIndex)
{
    Gen3BoxBufferManager boxBufferManager(saveMetadata_, saveReader_);
    u8 emptyData[SINGLE_MON_BYTES_IN_BOX];
    u32 personalityValue;

    memset(emptyData, 0, sizeof(emptyData));

     // seek to the start of the pokemon slot we want to remove
    const u32 boxStartOffset = boxIndex * (MONS_PER_BOX * SINGLE_MON_BYTES_IN_BOX);
    const u32 offsetWithinBox = pokemonIndex * SINGLE_MON_BYTES_IN_BOX;
    boxBufferManager.seek(boxStartOffset + offsetWithinBox);

    boxBufferManager.readUint32(personalityValue, Endianness::LITTLE);
    boxBufferManager.rewind(sizeof(u32));
    if(personalityValue == 0) // this slot is already empty, return false to indicate that we did not remove a pokemon
    {
        return false;
    }

    // overwrite the pokemon data with empty data to "remove" it from the box
    boxBufferManager.write(emptyData, SINGLE_MON_BYTES_IN_BOX); 

    return true;
}

bool Gen3SaveManager::isNationalDexUnlocked() const
{
    u32 offsetFieldA, offsetFieldB, offsetFieldC;
    u16 fieldA, fieldC;
    u8 fieldB;

    getNationalDexUnlockFieldOffsets(gameType_, offsetFieldA, offsetFieldB, offsetFieldC);
    
    seekToSectionOffset(saveReader_, saveMetadata_, 0, offsetFieldA);
    if(gameType_ == Game::FIRERED || gameType_ == Game::LEAFGREEN)
    {
        u8 fieldAFRLG;
        // in FRLG, the field is only 1 byte
        saveReader_.readUint8(fieldAFRLG);
        fieldA = fieldAFRLG;
    }
    else
    {
        saveReader_.readUint16(fieldA, Endianness::LITTLE);
    }

    seekToSectionOffset(saveReader_, saveMetadata_, 2, offsetFieldB);
    saveReader_.readUint8(fieldB);

    seekToSectionOffset(saveReader_, saveMetadata_, 2, offsetFieldC);
    saveReader_.readUint16(fieldC, Endianness::LITTLE);

    // https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_III)#National_Pok%C3%A9dex
    if(gameType_ == Game::FIRERED || gameType_ == Game::LEAFGREEN)
    {
        return (fieldA == 0xB9) && (fieldB & 6) && (fieldC == 0x6258);
    }
    else
    {
        return (fieldA & 0xDA00) && (fieldB & (1 << 6)) && (fieldC == 0x0302);
    }
}

void Gen3SaveManager::setNationalDexUnlocked(bool shouldBeUnlocked)
{
    u32 offsetFieldA, offsetFieldB, offsetFieldC;
    u8 fieldB;
    u16 newValue;
    const bool isFRLG = (gameType_ == Game::FIRERED || gameType_ == Game::LEAFGREEN);

    // https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_III)#National_Pok%C3%A9dex
    getNationalDexUnlockFieldOffsets(gameType_, offsetFieldA, offsetFieldB, offsetFieldC);

    seekToSectionOffset(saveReader_, saveMetadata_, 0, offsetFieldA);
    if(isFRLG)
    {
        newValue = shouldBeUnlocked ? 0xB9 : 0;
        saveReader_.writeUint8(static_cast<u8>(newValue));
    }
    else
    {
        newValue = shouldBeUnlocked ? 0xDA01 : 0;
        saveReader_.writeUint16(newValue, Endianness::LITTLE);
    }

    seekToSectionOffset(saveReader_, saveMetadata_, 2, offsetFieldB);
    saveReader_.readUint8(fieldB);

    if(shouldBeUnlocked)
    {
        fieldB |= (1 << 6); // set the bit that indicates national dex is unlocked
    }
    else
    {
        fieldB &= ~(1 << 6); // clear the bit that indicates national dex is unlocked
    }

    seekToSectionOffset(saveReader_, saveMetadata_, 2, offsetFieldB);
    saveReader_.writeUint8(fieldB);

    seekToSectionOffset(saveReader_, saveMetadata_, 2, offsetFieldC);
    if(!shouldBeUnlocked)
    {
        newValue = 0;
    }
    else
    {
        newValue = isFRLG ? 0x6258 : 0x0302;
    }

    saveReader_.writeUint16(newValue, Endianness::LITTLE);

    saveMetadata_.sectionModified_[0] = true;
    saveMetadata_.sectionModified_[2] = true;
}

bool Gen3SaveManager::isPokemonOwned(u16 speciesIndex) const
{
    return getBitFlag(saveReader_, saveMetadata_, 0, ownedFlagsBaseOffset, speciesIndex - 1);
}

bool Gen3SaveManager::isPokemonSeen(u16 speciesIndex) const
{
    const u32 flagIndex = speciesIndex - 1;
    u32 seenFlagsBaseOffsetA, seenFlagsBaseOffsetB, seenFlagsBaseOffsetC;

    getSeenFlagOffsetsForGame(gameType_, seenFlagsBaseOffsetA, seenFlagsBaseOffsetB, seenFlagsBaseOffsetC);

    return getBitFlag(saveReader_, saveMetadata_, 0, seenFlagsBaseOffsetA, flagIndex);
}

void Gen3SaveManager::setPokemonOwned(u16 speciesIndex, bool owned)
{
    const u16 ownedFlagsBaseOffset = 0x0028;
    setBitFlag(saveReader_, saveMetadata_, 0, ownedFlagsBaseOffset, speciesIndex - 1, owned);
}

void Gen3SaveManager::setPokemonSeen(u16 speciesIndex, bool seen)
{
    const u32 flagIndex = speciesIndex - 1;
    u32 seenFlagsBaseOffsetA, seenFlagsBaseOffsetB, seenFlagsBaseOffsetC;

    getSeenFlagOffsetsForGame(gameType_, seenFlagsBaseOffsetA, seenFlagsBaseOffsetB, seenFlagsBaseOffsetC);

    setBitFlag(saveReader_, saveMetadata_, 0, seenFlagsBaseOffsetA, flagIndex, seen);
    setBitFlag(saveReader_, saveMetadata_, 1, seenFlagsBaseOffsetB, flagIndex, seen);
    setBitFlag(saveReader_, saveMetadata_, 4, seenFlagsBaseOffsetC, flagIndex, seen);
}

void Gen3SaveManager::readTrainerName(u8 *outputBuffer)
{
    // trainer name is stored in the first section of the save file, starting at offset 0x0
    seekToSectionOffset(saveReader_, saveMetadata_, 0, 0);
    saveReader_.read(outputBuffer, 7); // trainer name is 7 bytes long
}

void Gen3SaveManager::finishSave()
{
    for(unsigned i = 0; i < NUM_BOX_SECTIONS; ++i)
    {
        if(saveMetadata_.sectionModified_[i])
        {
            updateSectionChecksum(saveReader_, saveMetadata_, i);
        }
    }
}

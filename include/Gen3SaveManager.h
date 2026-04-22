#ifndef _GEN3SAVEFILEMANAGER_H_
#define _GEN3SAVEFILEMANAGER_H_

#include "typeDefs.h"

static const unsigned NUM_SAVE_SECTIONS = 14;

typedef struct Gen3SaveMetadata
{
    u32 currentSaveOffset_;
    u32 sectionMap_[NUM_SAVE_SECTIONS];
    bool sectionModified_[NUM_SAVE_SECTIONS];
    u32 currentPcBoxIndex_;
} Gen3SaveMetadata;

class Gen3Pokemon;

/**
 * @brief This class allows you to manage a gen3 save file.
 * 
 * The Gen3SaveFileReaderType should be a class that functionally looks like this:
 * 
 * class IGen3SaveFileReader
 * {
 * public:
 *     virtual ~IGen3SaveFileReader() = default;
 * 
 *     virtual void read(u8 *buffer, u32 size) = 0;
 *     virtual void readUint8(u8& outByte) = 0;
 *     virtual void readUint16(u16& outWord, Endianness fieldEndianness) = 0;
 *     virtual void readUint32(u32& outDWord, Endianness fieldEndianness) = 0;
 * 
 *     virtual void write(const u8 *buffer, u32 size) = 0;
 *     virtual void writeUint8(u8 value) = 0;
 *     virtual void writeUint16(u16 value, Endianness fieldEndianness) = 0;
 *     virtual void writeUint32(u32 value, Endianness fieldEndianness) = 0;
 * 
 *     virtual void seek(u32 offset) = 0;
 *     virtual void advance(u32 numBytes) = 0;
 *     virtual void rewind(u32 numBytes) = 0;
 * 
 *     virtual bool shouldRecalculateChecksumsOnFinish() const = 0;
 * protected:
 * private:
 * };
 * 
 * I changed Gen3SaveManager into a template class because you'd only have 1 implementation of the save file reader
 * per project. (PTGB vs external project).
 * For optimization purposes, it's better if no virtual functions are involved.
 */
template <typename Gen3SaveFileReaderType>
class Gen3SaveManager
{
public:
    Gen3SaveManager(Game gameType, Language gameLanguage, Gen3SaveFileReaderType &saveReader);
    ~Gen3SaveManager();

    u32 getCurrentBoxIndex() const;

    u32 addPokemonToBox(int boxIndex, Gen3Pokemon& pokemon);
    bool removePokemonAtBoxIndex(int boxIndex, unsigned pokemonIndex);

    /**
     * @brief This function checks whether the National Dex has been unlocked in the save file.
     */
    bool isNationalDexUnlocked() const;
    /**
     * @brief This function unlocks the National Dex in the save file.
     */
    void setNationalDexUnlocked(bool shouldBeUnlocked);

    /**
     * @brief This function checks whether the Mystery Event feature has been unlocked.
     * Only relevant for R/S/E.
     */
    bool isMysteryEventUnlocked() const;

    /**
     * @brief This function unlocks the Mystery Event feature for R/S/E.
     */
    void setMysteryEventUnlocked(bool shouldBeUnlocked);

    /**
     * @brief This function checks whether the Mystery Gift feature has been unlocked. (relevant for FR/LG/E)
     */
    bool isMysteryGiftUnlocked() const;

    /**
     * @brief This function unlocks the Mystery Gift feature for FR/LG/E.
     */
    void setMysteryGiftUnlocked(bool shouldBeUnlocked);

    /**
     * @brief This function injects the specifid Mystery Event payload into the save file.
     * Note: it expects the data to be in the .me3 file format.
     * 
     * Note 2: This function WON'T unlock the mystery event feature automatically!
     */
    void injectMysteryEvent(const u8 *mysteryEvent3Data, u32 size);

    /**
     * @brief This function injects the specified Mystery Gift payload into the save file.
     * Note: it expects the data to be in the .wc3 file format.
     * 
     * Note 2: This function WON'T unlock the mystery gift feature automatically!
     */
    void injectMysteryGift(const u8 *mysteryGiftData, u32 size);

    bool isPokemonOwned(u16 speciesIndex) const;
    bool isPokemonSeen(u16 speciesIndex) const;
    void setPokemonOwned(u16 speciesIndex, bool owned);
    void setPokemonSeen(u16 speciesIndex, bool seen);

    /**
     * @brief This function reads the trainer name from the save file in the gen3 text encoding.
     * The output buffer should be at least 7 bytes long to hold the trainer name.
     */
    void readTrainerName(u8 *outputBuffer, u32 &outNameLength);

    void finishSave();
protected:
private:
    void seekToSectionOffset(const Gen3SaveMetadata& saveMetadata, u8 sectionId, u32 offsetWithinSection) const;
    u16 calculateSectionChecksum();
    /** 
    * Validates the current section checksum.
    * Note: the caller needs to seek to the start of the section.
    */
    bool validateSectionChecksum();
    void updateSectionChecksum(const Gen3SaveMetadata& saveMetadata, u8 sectionId);
    void getNationalDexUnlockFieldOffsets(Game game, u32& offsetFieldA, u32& offsetFieldB, u32& offsetFieldC) const;
    void getSeenFlagOffsetsForGame(Game game, u32& offsetFieldA, u32& offsetFieldB, u32& offsetFieldC) const;
    bool getBitFlag(const Gen3SaveMetadata& saveMetadata, u8 sectionId, u32 flagBaseOffset, u32 flagIndex) const;
    void setBitFlag(Gen3SaveMetadata& saveMetadata, u8 sectionId, u32 flagBaseOffset, u32 flagIndex, bool enabled);

    /**
     * @brief This function picks the latest save slot from the save file,
     * taking empty/corrupted slots into account.
     */
    u32 pickSaveSlot();

    /**
     * @brief This function searches through the save file to find the
     * current save slot and maps the sections within that save slot.
     */
    void indexSave(Gen3SaveMetadata& saveMetadata);

    Game gameType_;
    Language gameLanguage_;
    Gen3SaveMetadata saveMetadata_;
    Gen3SaveFileReaderType& saveReader_;
};

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
template <typename Gen3SaveFileReaderType>
class Gen3BoxBufferManager
{
public:
    Gen3BoxBufferManager(Gen3SaveMetadata& saveMetadata, Gen3SaveFileReaderType& saveReader);

    
    void seek(u32 offset_in_box_buffer);

    void rewind(u32 numBytes);

    void advance(u32 numBytes);

    void readUint32(u32& outDWord, Endianness fieldEndianness);

    // Note: this function will trigger a seek
    void read(u8* outBuffer, u32 numBytes);

    // Note: this function will trigger a seek
    void write(const u8* inBuffer, u32 numBytes);
protected:
private:
    u16 convertOffsetToSectionId(u32 offset) const;

    Gen3SaveMetadata &saveMetadata_;
    Gen3SaveFileReaderType &saveReader_;
    u32 curOffset_;
};

#include "Gen3SaveManagerImpl.h"

#endif
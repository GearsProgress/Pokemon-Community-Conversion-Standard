#ifndef _IGEN3SAVEMANAGER_H_
#define _IGEN3SAVEMANAGER_H_

#include "typeDefs.h"

class Gen3Pokemon;

/**
 * @brief This interface class helps to manage the save data of gen3 games.
 * It is used to abstract the method to access/modify the save file content.
 * For Poke_Transporter_GB, an implementation exists to read/write to the flash memory.
 * But for PCCS on other platforms, a file-based implementation exists.
 */
class IGen3SaveManager
{
public:
    virtual ~IGen3SaveManager() = default;

    /**
     * @brief Returns the index of the current box
     */
    virtual u32 getCurrentBoxIndex() const = 0;

    /**
     * @brief This function adds a gen3 pokémon to the box at the specified index.
     * If the box is full, we will abort and return UINT32_MAX.
     * Otherwise the pokémon will be added and the index within the box where it was added will be returned.
     */
    virtual unsigned addPokemonToBox(int boxIndex, Gen3Pokemon& pokemon) = 0;
    virtual bool removePokemonAtBoxIndex(int boxIndex, unsigned pokemonIndex) = 0;

    /**
     * @brief This function returns whether the national dex is unlocked in the save file.
     */
    virtual bool isNationalDexUnlocked(Game game) const = 0;

    /**
     * @brief This function will unlock the national dex
     */
    virtual void unlockNationalDex(Game game) = 0;

    /**
     * @brief This function should be called whenever we are done modifying the save file.
     * It will update the checksums of any modified sections.
     * 
     * WARNING: the save slot will be corrupted if you forget to call this function!
     */
    virtual void finishSave() = 0;
protected:
private:
};

#endif
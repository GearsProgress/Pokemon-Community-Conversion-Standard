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
class IGen3SaveFileReader;

/**
 * @brief This class allows you to manage a gen3 save file.
 */
class Gen3SaveManager
{
public:
    Gen3SaveManager(Game gameType, IGen3SaveFileReader &saveReader);
    ~Gen3SaveManager();

    u32 getCurrentBoxIndex() const;

    unsigned addPokemonToBox(int boxIndex, Gen3Pokemon& pokemon);
    bool removePokemonAtBoxIndex(int boxIndex, unsigned pokemonIndex);
    
    bool isNationalDexUnlocked() const;
    void setNationalDexUnlocked(bool shouldBeUnlocked);

    bool isPokemonOwned(u16 speciesIndex) const;
    bool isPokemonSeen(u16 speciesIndex) const;
    void setPokemonOwned(u16 speciesIndex, bool owned);
    void setPokemonSeen(u16 speciesIndex, bool seen);

    void finishSave();
protected:
private:
    Game gameType_;
    Gen3SaveMetadata saveMetadata_;
    IGen3SaveFileReader& saveReader_;
};

#endif
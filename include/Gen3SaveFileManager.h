#ifndef _GEN3SAVEFILEMANAGER_H_
#define _GEN3SAVEFILEMANAGER_H_

#include "IGen3SaveManager.h"
#include <cstdio>
#include <cstdint>

static const unsigned NUM_SAVE_SECTIONS = 14;

typedef struct Gen3SaveMetadata
{
    uint32_t currentSaveOffset_;
    uint32_t sectionMap_[NUM_SAVE_SECTIONS];
    bool sectionModified_[NUM_SAVE_SECTIONS];
    u32 currentPcBoxIndex_;
} Gen3SaveMetadata;

/**
 * @brief This class allows you to manage a gen3 save file.
 */
class Gen3SaveFileManager : public IGen3SaveManager
{
public:
    Gen3SaveFileManager(FILE *saveFile);
    virtual ~Gen3SaveFileManager();

    u32 getCurrentBoxIndex() const override;

    unsigned addPokemonToBox(int boxIndex, Gen3Pokemon& pokemon) override;
    bool removePokemonAtBoxIndex(int boxIndex, unsigned pokemonIndex) override;
    
    bool isNationalDexUnlocked(Game game) const override;
    void unlockNationalDex(Game game) override;

    void finishSave() override;
protected:
private:
    Gen3SaveMetadata saveMetadata_;
    FILE* saveFile_;
};

#endif
#include "Gen3SaveManager.h"
#include "Gen3SaveFileReader.h"
#include "Gen3Pokemon.h"
#include "typeDefs.h"
#include "pccs_utils.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>

#define BOOL_TO_STRING(x) ((x) ? "Yes" : "No")

static void print_usage()
{
    printf("Usage: unlockMysteryGift <save_file_path> <game_type> <game_language>\n");
}

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        print_usage();
        return EXIT_FAILURE;
    }

    FILE *savFile = fopen(argv[1], "r+b");
    if(!savFile)
    {
        perror("Could not open save file:");
        return EXIT_FAILURE;
    }

    const Game game = PCCSUtils::determineGameType(argv[2]);
    if(game == GAME_UNKNOWN)
    {
        printf("Invalid game type provided. Valid options are: ruby, sapphire, emerald, firered, leafgreen\n");
        return EXIT_FAILURE;
    }

    const Language language = PCCSUtils::determineLanguage(argv[3]);
    if(language == LANGUAGE_UNKNOWN)
    {
        printf("Invalid game language provided. Valid options are: english, japanese, french, german, italian, spanish\n");
        return EXIT_FAILURE;
    }

    Gen3SaveFileReader saveFileReader(savFile);
    Gen3SaveManager saveManager(game, language, saveFileReader);

    saveManager.setMysteryEventUnlocked(true);
    saveManager.setMysteryGiftUnlocked(true);
    saveManager.finishSave();

    const bool isMysteryEventUnlocked = saveManager.isMysteryEventUnlocked();
    const bool isMysteryGiftUnlocked = saveManager.isMysteryGiftUnlocked();

    fclose(savFile);

    printf("Unlock status: Mystery Event: %s, Mystery Gift: %s\n", BOOL_TO_STRING(isMysteryEventUnlocked), BOOL_TO_STRING(isMysteryGiftUnlocked));

    return EXIT_SUCCESS;
}
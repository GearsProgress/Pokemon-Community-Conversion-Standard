#include "Gen3SaveManager.h"
#include "Gen3SaveFileReader.h"
#include "Gen3Pokemon.h"
#include "typeDefs.h"
#include "pccs_utils.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <strings.h>

#define BOOL_TO_STRING(x) ((x) ? "Yes" : "No")

static bool is_truthy_string(const char* str)
{
    return strcasecmp(str, "true") == 0 || strcmp(str, "1") == 0;
}

static void print_usage()
{
    printf("Usage: unlockMysteryEvent <save_file_path> <game_type> <game_language> [shouldBeUnlocked]\n");
}

int main(int argc, char **argv)
{
    bool shouldBeUnlocked = true;
    if(argc < 4 || argc > 5)
    {
        print_usage();
        return EXIT_FAILURE;
    }

    const Game game = PCCSUtils::determineGameType(argv[2]);
    if(game == GAME_UNKNOWN)
    {
        printf("Invalid game type provided. Valid options are: ruby, sapphire, emerald, firered, leafgreen\n");
        return EXIT_FAILURE;
    }

    if(game == FIRERED || game == LEAFGREEN)
    {
        printf("The Mystery Event feature is not available in Fire Red and Leaf Green. Check Mystery Gift instead!\n");
        return EXIT_SUCCESS;
    }

    const Language language = PCCSUtils::determineLanguage(argv[3]);
    if(language == LANGUAGE_UNKNOWN)
    {
        printf("Invalid game language provided. Valid options are: english, japanese, french, german, italian, spanish\n");
        return EXIT_FAILURE;
    }

    if(argc == 5 && !is_truthy_string(argv[4]))
    {
        shouldBeUnlocked = false;
    }

    FILE *savFile = fopen(argv[1], "r+b");
    if(!savFile)
    {
        perror("Could not open save file:");
        return EXIT_FAILURE;
    }

    Gen3SaveFileReader saveFileReader(savFile);
    Gen3SaveManager saveManager(game, language, saveFileReader);

    saveManager.setMysteryEventUnlocked(shouldBeUnlocked);
    saveManager.finishSave();

    const bool isMysteryEventUnlocked = saveManager.isMysteryEventUnlocked();

    fclose(savFile);

    printf("Unlock status: Mystery Event: %s\n", BOOL_TO_STRING(isMysteryEventUnlocked));

    return EXIT_SUCCESS;
}

#include "Gen3SaveManager.h"
#include "Gen3SaveFileReader.h"
#include "Gen3Pokemon.h"
#include "typeDefs.h"
#include "pccs_utils.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>

static void print_usage()
{
    printf("Usage: isNationalDexUnlocked <save_file_path> <game_type>\n");
}

int main(int argc, char **argv)
{
    if(argc != 3)
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

    Gen3SaveFileReader saveFileReader(savFile);
    Gen3SaveManager saveManager(game, LANGUAGE_UNKNOWN, saveFileReader);
    const bool isNationalDexUnlocked = saveManager.isNationalDexUnlocked();

    fclose(savFile);

    if(isNationalDexUnlocked)
    {
        printf("The national dex is unlocked in this save file!\n");
    }
    else
    {
        printf("The national dex is NOT unlocked in this save file.\n");
    }

    return EXIT_SUCCESS;
}
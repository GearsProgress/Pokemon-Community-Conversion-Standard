#include "Gen3SaveManager.h"
#include "Gen3SaveFileReader.h"
#include "Gen3Pokemon.h"
#include "typeDefs.h"
#include "pccs_utils.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <strings.h>

static void print_usage()
{
    printf("Usage: isNationalDexUnlocked <save_file_path> <game_type>\n");
}

int main(int argc, char **argv)
{
    uint8_t pk3Buffer[100 * 1024];
    if(argc != 3)
    {
        print_usage();
        return EXIT_FAILURE;
    }

    const int boxIndex = std::atoi(argv[3]);

    FILE *savFile = fopen(argv[1], "r+b");
    if(!savFile)
    {
        perror("Could not open save file:");
        return EXIT_FAILURE;
    }

    const Game game = PCCSUtils::determineGameType(argv[2]);
    if(game == INVALID)
    {
        printf("Invalid game type provided. Valid options are: ruby, sapphire, emerald, firered, leafgreen\n");
        return EXIT_FAILURE;
    }

    Gen3SaveFileReader saveFileReader(savFile);
    Gen3SaveManager saveManager(game, saveFileReader);
    const bool isNationalDexUnlocked = saveManager.isNationalDexUnlocked();

    if(isNationalDexUnlocked)
    {
        printf("The national dex was already unlocked!\n");
        fclose(savFile);
        return EXIT_FAILURE;
    }
    
    saveManager.setNationalDexUnlocked(true);
    saveManager.finishSave();
    printf("The national dex has been unlocked!\n");
    fclose(savFile);

    return EXIT_SUCCESS;
}
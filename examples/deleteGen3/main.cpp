#include "Gen3SaveManager.h"
#include "Gen3SaveFileReader.h"
#include "Gen3Pokemon.h"
#include "pccs_utils.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>

static void print_usage()
{
    printf("Usage: deleteGen3FromBox <save_file_path> <game_type> <box_index> <index_in_box>\n");
}

int main(int argc, char **argv)
{
    if(argc != 5)
    {
        print_usage();
        return EXIT_FAILURE;
    }

    const int boxIndex = std::atoi(argv[3]);
    const int indexInBox = std::atoi(argv[4]);

    FILE *savFile = fopen(argv[1], "r+b");
    if(!savFile)
    {
        perror("Could not open save file:");
        return EXIT_FAILURE;
    }

    PokemonTables table;
    Gen3SaveFileReader saveFileReader(savFile);
    const Game game = PCCSUtils::determineGameType(argv[2]);
    Gen3SaveManager saveManager(game, saveFileReader);

    bool result = saveManager.removePokemonAtBoxIndex(boxIndex, indexInBox);
    saveManager.finishSave();

    fclose(savFile);

    if(result)
    {
        printf("Successfully removed Pokémon from box %d at index %d\n", boxIndex, indexInBox);
    }
    else
    {
        printf("Failed to remove Pokémon from box %d at index %d.\n", boxIndex, indexInBox);
    }

    return EXIT_SUCCESS;
}
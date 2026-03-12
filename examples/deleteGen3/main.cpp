#include "Gen3SaveFileManager.h"
#include "Gen3Pokemon.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>

static void print_usage()
{
    printf("Usage: deleteGen3FromBox <save_file_path> <box_index> <index_in_box>\n");
}

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        print_usage();
        return EXIT_FAILURE;
    }

    const int boxIndex = std::atoi(argv[2]);
    const int indexInBox = std::atoi(argv[3]);

    FILE *savFile = fopen(argv[1], "r+b");
    if(!savFile)
    {
        perror("Could not open save file:");
        return EXIT_FAILURE;
    }

    PokemonTables table;
    Gen3SaveFileManager saveManager(savFile);

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
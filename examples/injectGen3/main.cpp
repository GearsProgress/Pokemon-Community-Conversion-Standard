#include "Gen3SaveManager.h"
#include "Gen3SaveFileReader.h"
#include "Gen3Pokemon.h"
#include "pccs_utils.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>

static void print_usage()
{
    printf("Usage: injectGen3IntoBox <save_file_path> <gameType> <path/to/file.pk3> <box_index>\n");
}

static long readFileIntoBuffer(const char* filePath, uint8_t *buffer, size_t bufferSize)
{
    FILE* file = fopen(filePath, "rb");
    if(!file)
    {
        perror("Could not open file:");
        exit(EXIT_FAILURE);
    }
    
    fseek(file, 0L, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if(!fread(buffer, 1, fileSize, file))
    {
        fileSize = 0;
    }
    fclose(file);

    return fileSize;
}

int main(int argc, char **argv)
{
    uint8_t pk3Buffer[100 * 1024];
    if(argc != 5)
    {
        print_usage();
        return EXIT_FAILURE;
    }

    const int boxIndex = std::atoi(argv[4]);
    const long pk3FileSize = readFileIntoBuffer(argv[3], pk3Buffer, sizeof(pk3Buffer));

    FILE *savFile = fopen(argv[1], "r+b");
    if(!savFile)
    {
        perror("Could not open save file:");
        return EXIT_FAILURE;
    }

    PokemonTables table;
    Gen3SaveFileReader saveFileReader(savFile);
    const Game game = PCCSUtils::determineGameType(argv[2]);
    Gen3SaveManager saveManager(game, LANGUAGE_UNKNOWN, saveFileReader);
    Gen3Pokemon pokemon(&table);

    pokemon.loadData(pk3Buffer, false);

    unsigned result = saveManager.addPokemonToBox(boxIndex, pokemon);
    saveManager.finishSave();

    fclose(savFile);

    if(result != UINT32_MAX)
    {
        printf("Successfully injected Pokémon into box %d at slot %u\n", boxIndex, result + 1);
    }
    else
    {
        printf("Failed to inject Pokémon into box %d because the box is full\n", boxIndex);
    }

    return EXIT_SUCCESS;
}
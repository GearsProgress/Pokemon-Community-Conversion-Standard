#include "Gen3SaveFileManager.h"
#include "Gen3Pokemon.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>

static void print_usage()
{
    printf("Usage: injectGen3 <save_file_path> <path/to/file.pk3> <box_index>\n");
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

    (void)fread(buffer, 1, fileSize, file);
    fclose(file);

    return fileSize;
}

int main(int argc, char **argv)
{
    uint8_t pk3Buffer[100 * 1024];
    if(argc != 4)
    {
        print_usage();
        return EXIT_FAILURE;
    }

    const int boxIndex = std::atoi(argv[3]);
    const long pk3FileSize = readFileIntoBuffer(argv[2], pk3Buffer, sizeof(pk3Buffer));

    FILE *savFile = fopen(argv[1], "r+b");
    if(!savFile)
    {
        perror("Could not open save file:");
        return EXIT_FAILURE;
    }

    PokemonTables table;
    Gen3SaveFileManager saveManager(savFile);
    Gen3Pokemon pokemon(&table);

    pokemon.loadData(pk3Buffer, false);

    saveManager.addPokemonToBox(boxIndex, pokemon);
    saveManager.finishSave();

    fclose(savFile);

    return EXIT_SUCCESS;
}
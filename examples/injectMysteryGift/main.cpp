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
    printf("Usage: injectMysteryGift <save_file_path> <game_type> <game_language> <path/to/file.wc3>\n");
}

static u8* readFileIntoBuffer(const char* filePath, u32& outSize)
{
    FILE* file = fopen(filePath, "rb");
    if(!file)
    {
        perror("Could not open mystery gift file:");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    outSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8* outBuffer = new u8[outSize];
    if(fread(outBuffer, 1, outSize, file) != outSize)
    {
        perror("Could not read entire mystery gift file:");
        fclose(file);
        delete[] outBuffer;
        exit(EXIT_FAILURE);
    }

    fclose(file);
    return outBuffer;
}

int main(int argc, char **argv)
{
    if(argc != 5)
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

    if(game == RUBY || game == SAPPHIRE)
    {
        printf("The Mystery Gift feature is not available in Ruby and Sapphire. Check Mystery Event instead!\n");
        return EXIT_SUCCESS;
    }

    const Language language = PCCSUtils::determineLanguage(argv[3]);
    if(language == LANGUAGE_UNKNOWN)
    {
        printf("Invalid game language provided. Valid options are: english, japanese, french, german, italian, spanish\n");
        return EXIT_FAILURE;
    }

    FILE *savFile = fopen(argv[1], "r+b");
    if(!savFile)
    {
        perror("Could not open save file:");
        return EXIT_FAILURE;
    }

    u32 mysteryGiftDataSize;
    u8 *mysteryGiftData = readFileIntoBuffer(argv[4], mysteryGiftDataSize);

    Gen3SaveFileReader saveFileReader(savFile);
    Gen3SaveManager saveManager(game, language, saveFileReader);

    if(!saveManager.isMysteryGiftUnlocked())
    {
        saveManager.setMysteryGiftUnlocked(true);
        printf("The Mystery Gift has been unlocked in this save file.\n");
    }
    saveManager.injectMysteryGift(mysteryGiftData, mysteryGiftDataSize);
    saveManager.finishSave();

    fclose(savFile);
    delete[] mysteryGiftData;
    mysteryGiftData = nullptr;

    printf("Successfully injected the specified Mystery Gift into the save file!\n");

    return EXIT_SUCCESS;
}

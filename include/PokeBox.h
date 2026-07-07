#ifndef POKEBOX_H
#define POKEBOX_H

#include "Gen1Pokemon.h"
#include "Gen2Pokemon.h"
#include "Gen3Pokemon.h"

class PokeBox // Stores up to 30 Pokemon in a box
{
private:
    void convertPkmn(int index);
    PokemonTables *table;
    Pokemon *boxStorage[30];
    Pokemon *nullMon;
    int currIndex;

public:
    PokeBox(PokemonTables *nTable = nullptr);
    void setTable(PokemonTables *nTable);
    bool addPokemon(Pokemon *currPkmn);
    Pokemon *getPokemon(int index);
    GBPokemon *getGBPokemon(int index);
    Gen3Pokemon *getGen3Pokemon(int index);
    bool removePokemon(int index);
    void loadData(int generation, Language nLang, const byte nDataArray[]);
    void convertAll();
    int getNumInBox();
    int getNumValid();
    bool stabilize_mythical;

    bool getContainsMythical();
    bool getContainsInvalid();
    bool getContainsMissingNo();

#if ON_GBA
    bool getHasNewPkmn();
#else
    friend std::ostream &operator<<(std::ostream &os, PokeBox &pc)
    {
        for (int i = 0; i < pc.currIndex; i++)
        {
            os << "\n"
               << "---------------- " << "POKEMON #" << i << " ----------------" << "\n"
               << *pc.boxStorage[i] << "\n";
        }

        return os;
    }
    std::string printDataArray();
#endif
};

#endif
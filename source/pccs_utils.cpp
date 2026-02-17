#include "pccs_utils.h"

// the attribute stuff is to make sure the compiler doesn't optimize this code away.
__attribute__((optimize("O0")))
bool isCurrentCPULittleEndian()
{
	short int word = 0x0001;
   char *byte = (char *) &word;
   return (byte[0] ? true : false);
}

u16 byteSwapUint16(u16 val)
{
    return __builtin_bswap16(val);
}

u32 byteSwapUint32(u32 val)
{
	return __builtin_bswap32(val);
}

void extractLehmerCode4(u32 n, u8 out[4])
{
    u8 pool[4] = {0, 1, 2, 3};

    // factorials: (digitsRemaining - 1)!
    // with digitsRemaining the number of digits remaining
    // after we picked the current one
    const u8 fact[4] = {6, 2, 1, 1};
    
    // fact[pos] tells you how many permutations each choice at this position represents.

    for (u32 pos = 0; pos < 4; pos++)
    {
        // n / fact[pos] = “which element to pick among the remaining elements”
        u32 pick = n / fact[pos];
        n %= fact[pos];

        out[pos] = pool[pick];

        // remove picked element from pool
        for (u32 i = pick; i < 3 - pos; i++)
        {
            pool[i] = pool[i + 1];
        }
    }
}
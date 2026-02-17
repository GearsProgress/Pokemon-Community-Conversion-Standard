#ifndef PCCS_UTILS_H
#define PCCS_UTILS_H

#include "typeDefs.h"

bool isCurrentCPULittleEndian();

u16 byteSwapUint16(u16 val);
u32 byteSwapUint32(u32 val);

/**
 * This function extracts a 4 digit sequence from Lehmer code 'n'.
 * 'n' represents the Lehmer code, which is an index into all the permutations
 * The idea is that the Lehmer code can be thought of as a mixed radix number, 
 * where the radix for each position is the number of remaining digits to choose from.
 * 
 * For example, for the first position, there are 4 digits to choose from, so the radix is 4. 
 * For the second position, there are 3 digits left to choose from, so the radix is 3, and so on.
 * The function works by iteratively determining which digit to pick for each position based on the 
 * Lehmer code and the factorial of the number of remaining digits, and then removing that digit 
 * from the pool of available digits.
 * That way, we can convert the Lehmer code into a specific permutation of the digits 0, 1, 2, and 3.
 * 
 * And it turns out that the way the substructures are ordered in the Pokemon data is based on the permutation
 * of the numbers 0(G), 1(A), 2(E), and 3(M) that corresponds to the personality value modulo 24
 * (since there are 24 permutations of 4 digits).
 * 
 * It is thought that this is done to make it more difficult to reverse engineer the encryption, 
 * since the location of the various data substructures changes based on the personality value, 
 * which is a value that is not easily guessable and can be different for each Pokemon.
 * 
 * But using the Lehmer code, we can determine the order of the substructures and thus how to decrypt the data.
 * By calculating it, we can supposedly save on storage space since we don't have to store the substructure order
 * for each personality value, we can just calculate the order on the fly.
 * 
 * But it does make it quite a bit more complicated though.
 */
void extractLehmerCode4(u32 n, u8 out[4]);

#endif
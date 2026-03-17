#ifndef PCCS_UTILS_H
#define PCCS_UTILS_H

#include "typeDefs.h"
#include <cstring>

namespace PCCSUtils
{

bool isCurrentCPULittleEndian();

u16 byteSwapUint16(u16 val);
u32 byteSwapUint32(u32 val);

/**
 * @brief This function copies bytes from the srcBuffer to the dstBuffer using memcpy,,
 * and returns a pointer to the next byte in the srcBuffer after the copied bytes.
 * 
 * It's mostly used as a helper function for the other read functions in this namespace,
 * 
 * @param srcBuffer buffer to read from
 * @param dstBuffer buffer to write to
 * @param bytesToRead number of bytes to read
 * @return u8* pointer to the next byte in the source buffer after reading
 */
u8* read(u8 *srcBuffer, u8 *dstBuffer, u32 bytesToRead);

/**
 * @brief This function copies bytes from the srcBuffer to the dstBuffer using memcpy,,
 * and returns a pointer to the next byte in the dstBuffer after the copied bytes.
 * 
 * It's mostly used as a helper function for the other write functions in this namespace,
 * 
 * @param srcBuffer buffer to read from
 * @param dstBuffer buffer to write to
 * @param bytesToRead number of bytes to read
 * @return u8* pointer to the next byte in the destination buffer after writing
 */
u8* write(const u8* srcBuffer, u8 *dstBuffer, u32 bytesToWrite);

/**
 * @brief This function reads a 16 bit unsigned integer from the source buffer and stores it in outBytes
 * The advantage of this function is that if the current CPU endianness matches the field endianness, 
 * we can read the entire 16 bits as a single unit, which is more efficient than reading byte by byte.
 * 
 * If it doesn't, this function does the required byte swapping to convert the endianness of the data 
 * to match the CPU endianness, so that it can be read correctly.
 * 
 * @param srcBuffer buffer to read from
 * @param outBytes output variable
 * @param fieldEndianness the endianness of the data in the source buffer
 * @return u8* pointer to the next byte in the source buffer after reading
 */
u8* readUint16(u8 *srcBuffer, u16& outBytes, Endianness fieldEndianness);

/**
 * @brief This function writes a 16 bit unsigned integer to dstBuffer from the bytes variable.
 * The advantage of this function is that if the current CPU endianness matches the field endianness,
 * we can write the entire 16 bits as a single unit, which is more efficient than writing byte by byte.
 * 
 * If the endianness between the CPU and fieldEndianness doesn't match, this function does the required byte 
 * swapping to convert the endianness of the data, so it will be written correctly.
 * 
 * @param dstBuffer buffer to write to
 * @param bytes input variable
 * @param fieldEndianness endianness of how the data should be represented in dstBuffer
 * @return u8* pointer to the next byte in the destination buffer after writing
 */
u8* writeUint16(u8 *dstBuffer, u16 bytes, Endianness fieldEndianness);

/**
 * @brief This function reads a 24 bit unsigned integer from the source buffer and stores it in outBytes
 * 
 * This is a bit of an odd function, because it reads 3 bytes, but stores it in a 32 bit unsigned integer.
 * 
 * This function does the required byte swapping to convert the endianness of the data 
 * to match the CPU endianness, so that it can be read correctly.
 * 
 * @param srcBuffer buffer to read from
 * @param outBytes output variable
 * @param fieldEndianness the endianness of the data in the source buffer
 * @return u8* pointer to the next byte in the source buffer after reading
 */
u8* readUint24(u8 *srcBuffer, u32& outByte, Endianness fieldEndianness);

/**
 * @brief This function writes a 24 bit unsigned integer to dstBuffer from the bytes variable.
 * 
 * This is a bit of an odd function, because it writes 3 bytes, but takes in a 32 bit unsigned integer as input.
 * Obviously the upper byte is ignored, but it's easier to just take in a u32 since there isn't really a 24 bit unsigned integer type.
 * 
 * This function does the required byte swapping to convert the endianness of the data 
 * to match the CPU endianness, so that it can be written correctly.
 * 
 * @param dstBuffer buffer to write to
 * @param bytes input variable
 * @param fieldEndianness endianness of how the data should be represented in dstBuffer
 * @return u8* pointer to the next byte in the destination buffer after writing
 */
u8* writeUint24(u8 *dstBuffer, u32 bytes, Endianness fieldEndianness);

/**
 * @brief This function reads a 32 bit unsigned integer from the source buffer and stores it in outBytes
 * The advantage of this function is that if the current CPU endianness matches the field endianness, 
 * we can read the entire 32 bits as a single unit, which is more efficient than reading byte by byte.
 * 
 * If it doesn't, this function does the required byte swapping to convert the endianness of the data 
 * to match the CPU endianness, so that it can be read correctly.
 * 
 * @param srcBuffer buffer to read from
 * @param outBytes output variable
 * @param fieldEndianness the endianness of the data in the source buffer
 * @return u8* pointer to the next byte in the source buffer after reading
 */
u8* readUint32(u8 *srcBuffer, u32& outByte, Endianness fieldEndianness);

/**
 * @brief This function writes a 32 bit unsigned integer to dstBuffer from the bytes variable.
 * The advantage of this function is that if the current CPU endianness matches the field endianness,
 * we can write the entire 32 bits as a single unit, which is more efficient than writing byte by byte.
 * 
 * If the endianness between the CPU and fieldEndianness doesn't match, this function does the required byte 
 * swapping to convert the endianness of the data, so it will be written correctly.
 * 
 * @param dstBuffer buffer to write to
 * @param bytes input variable
 * @param fieldEndianness endianness of how the data should be represented in dstBuffer
 * @return u8* pointer to the next byte in the destination buffer after writing
 */
u8* writeUint32(u8 *dstBuffer, u32 bytes, Endianness fieldEndianness);

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

/**
 * @brief Determine a game type based on a string parameter. The comparison is case-insensitive.
 * This is useful for cmdline tools.
 */
Game determineGameType(const char* param);

}

#endif
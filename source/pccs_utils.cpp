#include "pccs_utils.h"

static const bool g_cpuLittleEndian = PCCSUtils::isCurrentCPULittleEndian();

// the attribute stuff is to make sure the compiler doesn't optimize this code away.
__attribute__((optimize("O0")))
bool PCCSUtils::isCurrentCPULittleEndian()
{
	short int word = 0x0001;
   char *byte = (char *) &word;
   return (byte[0] ? true : false);
}

u16 PCCSUtils::byteSwapUint16(u16 val)
{
    return __builtin_bswap16(val);
}

u32 PCCSUtils::byteSwapUint32(u32 val)
{
	return __builtin_bswap32(val);
}

u8* PCCSUtils::read(u8 *srcBuffer, u8 *dstBuffer, u32 bytesToRead)
{
    memcpy(dstBuffer, srcBuffer, bytesToRead);
    return srcBuffer + bytesToRead;
}

u8* PCCSUtils::write(const u8* srcBuffer, u8 *dstBuffer, u32 bytesToWrite)
{
    memcpy(dstBuffer, srcBuffer, bytesToWrite);
    return dstBuffer + bytesToWrite;
}

u8* PCCSUtils::readUint16(u8 *srcBuffer, u16& outBytes, Endianness fieldEndianness)
{
    if(!PCCSUtils::read(srcBuffer, (u8*)&outBytes, 2))
    {
        return nullptr;
    }

    if(fieldEndianness == Endianness::LITTLE && !g_cpuLittleEndian)
    {
        outBytes = PCCSUtils::byteSwapUint16(outBytes);
    }
    else if(fieldEndianness == Endianness::BIG && g_cpuLittleEndian)
    {
        outBytes = PCCSUtils::byteSwapUint16(outBytes);
    }
    return srcBuffer + 2;
}

u8* PCCSUtils::writeUint16(u8 *dstBuffer, u16 bytes, Endianness fieldEndianness)
{
    // when reading the data before, we might have done a byte swap. To undo it while writing, we just need to repeat it.
    if(fieldEndianness == Endianness::LITTLE && !g_cpuLittleEndian)
    {
        bytes = PCCSUtils::byteSwapUint16(bytes);
    }
    else if(fieldEndianness == Endianness::BIG && g_cpuLittleEndian)
    {
        bytes = PCCSUtils::byteSwapUint16(bytes);
    }
    PCCSUtils::write((u8*)&bytes, dstBuffer, 2);
    return dstBuffer + 2;
}

u8* PCCSUtils::readUint24(u8 *srcBuffer, u32& outByte, Endianness fieldEndianness)
{
    u8 bytes[3];

    if(!PCCSUtils::read(srcBuffer, bytes, 3))
    {
        return nullptr;
    }

    if(fieldEndianness == Endianness::LITTLE)
    {
        outByte = (((u32)bytes[2]) << 16) | (((u32)bytes[1]) << 8) | bytes[0];
    }
    else
    {
        outByte = (((u32)bytes[0]) << 16) | (((u32)bytes[1]) << 8) | bytes[2];
    }

    return srcBuffer + 3;
}

u8* PCCSUtils::writeUint24(u8 *dstBuffer, u32 bytes, Endianness fieldEndianness)
{
    u8 bytesToWrite[3];

    if(fieldEndianness == Endianness::LITTLE)
    {
        bytesToWrite[0] = (u8)(bytes & 0xFF);
        bytesToWrite[1] = (u8)((bytes & 0xFF00) >> 8);
        bytesToWrite[2] = (u8)((bytes & 0xFF0000) >> 16);
    }
    else
    {
        bytesToWrite[0] = (u8)((bytes & 0xFF0000) >> 16);
        bytesToWrite[1] = (u8)((bytes & 0xFF00) >> 8);
        bytesToWrite[2] = (u8)(bytes & 0xFF);   
    }

    PCCSUtils::write(bytesToWrite, dstBuffer, sizeof(bytesToWrite));
    return dstBuffer + 3;
}

u8* PCCSUtils::readUint32(u8 *srcBuffer, u32& outByte, Endianness fieldEndianness)
{
    if(!PCCSUtils::read(srcBuffer, (u8*)&outByte, 4))
    {
        return nullptr;
    }

    if(fieldEndianness == Endianness::LITTLE && !g_cpuLittleEndian)
    {
        outByte = PCCSUtils::byteSwapUint32(outByte);
    }
    else if(fieldEndianness == Endianness::BIG && g_cpuLittleEndian)
    {
        outByte = PCCSUtils::byteSwapUint32(outByte);
    }
    return srcBuffer + 4;
}

u8* PCCSUtils::writeUint32(u8 *dstBuffer, u32 bytes, Endianness fieldEndianness)
{
    // when reading the data before, we might have done a byte swap. To undo it while writing, we just need to repeat it.
    if(fieldEndianness == Endianness::LITTLE && !g_cpuLittleEndian)
    {
        bytes = PCCSUtils::byteSwapUint32(bytes);
    }
    else if(fieldEndianness == Endianness::BIG && g_cpuLittleEndian)
    {
        bytes = PCCSUtils::byteSwapUint32(bytes);
    }
    PCCSUtils::write((u8*)&bytes, dstBuffer, 4);
    return dstBuffer + 4;
}

void PCCSUtils::extractLehmerCode4(u32 n, u8 out[4])
{
    u32 pool[4] = {0, 1, 2, 3};

    // factorials: (digitsRemaining - 1)!
    // with digitsRemaining the number of digits remaining
    // after we picked the current one
    const u32 fact[4] = {6, 2, 1, 1};
    
    // fact[pos] tells you how many permutations each choice at this position represents.
    // using u32 because that's the native register size for a GBA. #perf
    for (u32 pos = 0; pos < 4; ++pos)
    {
        // n / fact[pos] = “which element to pick among the remaining elements”
        u32 pick = n / fact[pos];
        n %= fact[pos];

        out[pos] = pool[pick];

        // remove picked element from pool
        for (u32 i = pick; i < 3 - pos; ++i)
        {
            pool[i] = pool[i + 1];
        }
    }
}

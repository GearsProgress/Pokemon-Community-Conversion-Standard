#ifndef _IGEN3SAVEFILEREADER_H_
#define _IGEN3SAVEFILEREADER_H_

#include "typeDefs.h"

/**
 * @brief This interface class helps to manage the save data of gen3 games.
 * It is used to abstract the method to access/modify the save file content.
 * For Poke_Transporter_GB, an implementation exists to read/write to the flash memory.
 * But for PCCS on other platforms, a file-based implementation exists.
 */
class IGen3SaveFileReader
{
public:
    virtual ~IGen3SaveFileReader() = default;

    virtual void read(u8 *buffer, u32 size) = 0;
    virtual void readUint8(u8& outByte) = 0;
    virtual void readUint16(u16& outWord, Endianness fieldEndianness) = 0;
    virtual void readUint32(u32& outDWord, Endianness fieldEndianness) = 0;

    virtual void write(const u8 *buffer, u32 size) = 0;
    virtual void writeUint8(u8 value) = 0;
    virtual void writeUint16(u16 value, Endianness fieldEndianness) = 0;
    virtual void writeUint32(u32 value, Endianness fieldEndianness) = 0;

    virtual void seek(u32 offset) = 0;
    virtual void advance(u32 numBytes) = 0;
    virtual void rewind(u32 numBytes) = 0;
protected:
private:
};

#endif
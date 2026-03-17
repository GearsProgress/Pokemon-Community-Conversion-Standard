#ifndef _GEN3SAVEFILEREADER_H_
#define _GEN3SAVEFILEREADER_H_

#include "IGen3SaveFileReader.h"
#include <cstdio>

/**
 * @brief File-based implementation of IGen3SaveFileReader. 
 * This is used for PCCS on platforms other than the GBA.
 */
class Gen3SaveFileReader : public IGen3SaveFileReader
{
public:
    Gen3SaveFileReader(FILE* saveFile);
    virtual ~Gen3SaveFileReader();

    void read(u8 *buffer, u32 size) override;
    void readUint8(u8& outByte) override;
    void readUint16(u16& outWord, Endianness fieldEndianness) override;
    void readUint32(u32& outDWord, Endianness fieldEndianness) override;

    void write(const u8 *buffer, u32 size) override;
    void writeUint8(u8 value) override;
    void writeUint16(u16 value, Endianness fieldEndianness) override;
    void writeUint32(u32 value, Endianness fieldEndianness) override;

    void seek(u32 offset) override;
    void advance(u32 numBytes) override;
    void rewind(u32 numBytes) override;
protected:
private:
    FILE* saveFile_;
    u32 curOffset_;
};

#endif
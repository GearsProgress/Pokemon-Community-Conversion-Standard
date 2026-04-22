#ifndef _GEN3SAVEFILEREADER_H_
#define _GEN3SAVEFILEREADER_H_

#include "typeDefs.h"
#include <cstdio>

/**
 * @brief File-based implementation of a save file reader that can be used by Gen3SaveManager. 
 * This is used for PCCS on platforms other than the GBA.
 */
class Gen3SaveFileReader
{
public:
    Gen3SaveFileReader(FILE* saveFile);
    ~Gen3SaveFileReader();

    void read(u8 *buffer, u32 size);
    void readUint8(u8& outByte);
    void readUint16(u16& outWord, Endianness fieldEndianness);
    void readUint32(u32& outDWord, Endianness fieldEndianness);

    void write(const u8 *buffer, u32 size);
    void writeUint8(u8 value);
    void writeUint16(u16 value, Endianness fieldEndianness);
    void writeUint32(u32 value, Endianness fieldEndianness);

    void seek(u32 offset);
    void advance(u32 numBytes);
    void rewind(u32 numBytes);

    /**
     * @brief Indicates whether the Gen3SaveManager should recalculate checksums when finishSave is called.
     */
    bool shouldRecalculateChecksumsOnFinish() const;
protected:
private:
    FILE* saveFile_;
    u32 curOffset_;
};

#endif
#include "pccs_settings.h"

#if !ON_GBA
#include "Gen3SaveFileReader.h"
#include "pccs_utils.h"

Gen3SaveFileReader::Gen3SaveFileReader(FILE* saveFile)
    : saveFile_(saveFile)
    , curOffset_(0)
{
}

Gen3SaveFileReader::~Gen3SaveFileReader()
{
}

void Gen3SaveFileReader::read(u8 *buffer, u32 size)
{
    if(!fread(buffer, 1, size, saveFile_))
    {
        return;
    }
    curOffset_ += size;
}

void Gen3SaveFileReader::readUint8(u8& outByte)
{
    if(!fread(&outByte, 1, 1, saveFile_))
    {
        return;
    }
    curOffset_ += 1;
}

void Gen3SaveFileReader::readUint16(u16& outWord, Endianness fieldEndianness)
{
    u8 buffer[2];
    if(!fread(buffer, 1, sizeof(buffer), saveFile_))
    {
        return;
    }
    PCCSUtils::readUint16(buffer, outWord, fieldEndianness);
    curOffset_ += sizeof(buffer);
}

void Gen3SaveFileReader::readUint32(u32& outDWord, Endianness fieldEndianness)
{
    u8 buffer[4];
    if(!fread(buffer, 1, sizeof(buffer), saveFile_))
    {
        return;
    }
    PCCSUtils::readUint32(buffer, outDWord, fieldEndianness);
    curOffset_ += sizeof(buffer);
}

void Gen3SaveFileReader::write(const u8 *buffer, u32 size)
{
    fwrite(buffer, 1, size, saveFile_);
    curOffset_ += size;
}

void Gen3SaveFileReader::writeUint8(u8 value)
{
    fwrite(&value, 1, sizeof(value), saveFile_);
    curOffset_ += sizeof(value);
}

void Gen3SaveFileReader::writeUint16(u16 value, Endianness fieldEndianness)
{
    u8 buffer[2];
    PCCSUtils::writeUint16(buffer, value, fieldEndianness);
    fwrite(buffer, 1, sizeof(buffer), saveFile_);
    curOffset_ += sizeof(buffer);
}

void Gen3SaveFileReader::writeUint32(u32 value, Endianness fieldEndianness)
{
    u8 buffer[4];
    PCCSUtils::writeUint32(buffer, value, fieldEndianness);
    fwrite(buffer, 1, sizeof(buffer), saveFile_);
    curOffset_ += sizeof(buffer);
}

void Gen3SaveFileReader::seek(u32 offset)
{
    fseek(saveFile_, offset, SEEK_SET);
    curOffset_ = offset;
}

void Gen3SaveFileReader::advance(u32 numBytes)
{
    fseek(saveFile_, numBytes, SEEK_CUR);
    curOffset_ += numBytes;
}

void Gen3SaveFileReader::rewind(u32 numBytes)
{
    fseek(saveFile_, -static_cast<long>(numBytes), SEEK_CUR);
    curOffset_ -= numBytes;
}

bool Gen3SaveFileReader::shouldRecalculateChecksumsOnFinish() const
{
    return true;
}
#endif
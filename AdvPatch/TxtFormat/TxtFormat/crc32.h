

// Original author of this file is notwa@github
// Redistributed by wiki908@github

#pragma once

#define CRC_STARTING   (0xFFFFFFFF)  //
#define CRC_POLYNOMIAL (0x04C11DB7)  // CRC 的默认系数


class CRC32
{
    typedef unsigned long ulong;
    enum { CRC_TABLE_SIZE = 0x100 };

    void crc_fill_table(ulong *table, int big, ulong polynomial);
    void crc_be_cycle(ulong *table, ulong *remainder, char c);
    void crc_le_cycle(ulong *table, ulong *remainder, char c);
    ulong crc_reflect(ulong input);

    ulong starting;
    char big_endian;
    ulong polynomial;
    char xor_output;
    char reflect_output;

    char init_done;

    ulong crc_table[CRC_TABLE_SIZE];

public:
    CRC32();
    void Init(const char xorOutput = 1, const char bigEndian = 0, const char reflectOutput = 0, 
        const ulong Starting = CRC_STARTING, const ulong Polynomical = CRC_POLYNOMIAL);
    ulong Calc(const char* buff, const long length);
};

inline CRC32::ulong CRC32::crc_reflect(ulong input)
{
    ulong reflected = 0;
    int i;
    for (i = 0; i < 4 * 8; i++) {
        reflected <<= 1;
        reflected |= input & 1;
        input >>= 1;
    }
    return reflected;
}

inline void CRC32::crc_fill_table(ulong *table, int big, ulong Polynomial)
{
    ulong lsb = (big) ? 1 << 31 : 1; /* least significant bit */
    ulong poly = (big) ? Polynomial : crc_reflect(Polynomial);
    int c, i;

    for (c = 0; c < CRC_TABLE_SIZE; c++, table++) {
        *table = (big) ? c << 24 : c;
        for (i = 0; i < 8; i++) {
            if (*table & lsb) {
                *table = (big) ? *table << 1 : *table >> 1;
                *table ^= poly;
            }
            else {
                *table = (big) ? *table << 1 : *table >> 1;
            }
            *table &= 0xFFFFFFFF;
        }
    }
}

inline void CRC32::crc_be_cycle(ulong *table, ulong *remainder, char c)
{
    ulong byte = table[(((*remainder) >> 24) ^ c) & 0xff];
    *remainder = (((*remainder) << 8) ^ byte) & 0xFFFFFFFF;
}

inline void CRC32::crc_le_cycle(ulong *table, ulong *remainder, char c)
{
    ulong byte = table[((*remainder) ^ c) & 0xFF];
    *remainder = ((*remainder) >> 8) ^ byte;
}

inline CRC32::CRC32()
{
    Init();
}

inline void CRC32::Init(const char xorOutput, const char bigEndian, const char reflectOutput,
    const ulong Starting, const ulong Polynomical)
{
    this->starting = Starting;
    this->polynomial = Polynomical;
    this->big_endian = bigEndian;
    this->xor_output = xorOutput;
    this->reflect_output = reflectOutput;

    crc_fill_table(crc_table, big_endian, polynomial);
}

inline CRC32::ulong CRC32::Calc(const char* buff, const long length)
{
    ulong remainder = starting;

    //crc_fill_table(crc_table, big_endian, Polynomial);

    if (big_endian)
        for (int i = 0; i < length; i++)
            crc_be_cycle(crc_table, &remainder, buff[i]);
    else
        for (int i = 0; i < length; i++)
            crc_le_cycle(crc_table, &remainder, buff[i]);

    if (xor_output)
        remainder ^= 0xFFFFFFFF;
    if (reflect_output)
        remainder = crc_reflect(remainder);
    return remainder;
}

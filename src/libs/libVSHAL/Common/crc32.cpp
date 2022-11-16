#include "crc32.h"

//See crc32.h for copyright information

namespace LM_VSHAL
{
    #define DO1(buf) a_OldCRC = CRC_TABLE[((int)a_OldCRC ^ (*a_Data++)) & 0xff] ^ (a_OldCRC >> 8);
    #define DO2(buf)  DO1(buf); DO1(buf);
    #define DO4(buf)  DO2(buf); DO2(buf);
    #define DO8(buf)  DO4(buf); DO4(buf);



    uint32_t computeCRC(uint32_t a_OldCRC, uint8_t *a_Data, uint32_t a_Length)
    {
        a_OldCRC = a_OldCRC ^ 0xffffffffL;
        while (a_Length >= 8)
        {
            DO8(a_Data);
            a_Length -= 8;
        }
        if (a_Length) do
        {
            DO1(a_Data);
        } while (--a_Length);
        return a_OldCRC ^ 0xffffffffL;
    }
}

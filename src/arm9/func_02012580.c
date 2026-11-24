#include "types.h"

extern u16 data_0203b960[];
extern u16 data_0203b8a0[];

u16 func_02012580(u32 c)
{
    u16 i;

    if (c >= 0x20 && c <= 0x7f)
        return c - 0x20;
    if (c >= 0xa0 && c <= 0xff)
        return c - 0x30;
    if (c >= 0x3040 && c <= 0x309f)
        return c - 0x2f40;
    if (c >= 0x30a0 && c <= 0x30ff)
        return c - 0x2f30;
    if (c >= 0xe000 && c <= 0xe028)
        return c - 0xddb0;
    for (i = 0; i < 0x40; i++) {
        if (c == data_0203b960[i])
            return i + 0x200;
    }
    for (i = 0; i < 0x20; i++) {
        if (c == data_0203b8a0[i])
            return i + 0x290;
    }
    return 0x1f;
}
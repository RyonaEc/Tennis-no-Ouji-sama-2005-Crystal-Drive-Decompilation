#include "types.h"

typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *data_ov002_02102070;

void func_ov002_020ace70(void)
{
    u8 *p = data_ov002_02102070 + 0x1824;

    *(u32 *)(p + 0x5c) = 0;
    *(u32 *)(p + 0x60) = 0;
    *(u32 *)(p + 0x64) = 0;
    *(u32 *)(p + 0x68) = 0;
    *(u32 *)(p + 0x6c) = 0;
    *(u32 *)(p + 0x70) = 0;
    *(u32 *)(p + 0xb8) &= ~0x3f0;
}
#include "types.h"

extern u32 data_ov002_02102070[];
extern u8 data_02042a9c[];

void func_ov002_0205f198(s32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *e = base + 0x2e4 + a0 * 0x158;
    u16 m = *(u16 *)(base + 6);
    u16 v0 = *(u16 *)(data_02042a9c + a0 * 4);
    u32 v1 = *(u32 *)(base + a0 * 4 + 0x264);

    if (m == 7) {
        *(u16 *)(e + 0x52) = v0 & (v0 ^ *(u16 *)(e + 0x50));
        *(u16 *)(e + 0x50) = v0;
        *(u32 *)(e + 0x5c) = v1 & (v1 ^ *(u32 *)(e + 0x58));
        *(u32 *)(e + 0x58) = v1;
    } else {
        *(u16 *)(e + 0x52) = 0;
        *(u16 *)(e + 0x50) = 0;
        *(u32 *)(e + 0x5c) = 0;
        *(u32 *)(e + 0x58) = 0;
    }
}
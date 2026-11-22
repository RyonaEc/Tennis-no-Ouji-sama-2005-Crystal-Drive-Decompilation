#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_ov002_020848b8(void);

void func_ov002_02089484(u32 a0)
{
    u8 *o = (u8 *)func_ov002_020848b8();
    u8 *b = *(u8 **)data_ov002_02102070 + 0x4bc4;

    *(u32 *)o |= 0x300;
    b += 0x300;
    *(u16 *)(o + 0xc) = *(u16 *)(b + 0x58);
    *(u16 *)(o + 0x14) = 0;
    *(u16 *)(o + 0x16) = 0;
    *(u16 *)(o + 0xe) = *(u16 *)(b + 0x5a);
    *(u16 *)(o + 0x18) = a0 & 0x1ff;
    *(u16 *)(o + 0x1a) = 0;
}
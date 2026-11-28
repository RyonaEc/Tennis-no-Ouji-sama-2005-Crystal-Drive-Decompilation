#include "types.h"

extern u8 data_ov003_020eb720[];
extern u32 func_02021280(u32 a0);

u32 func_ov003_020b6b94(u32 a0)
{
    u8 *base = *(u8 **)data_ov003_020eb720;

    switch (a0) {
    case 0:
        func_02021280(2);
        *(u16 *)0x0400000a = (*(u16 *)0x0400000a & 0x43) | 0x1f18;
        *(u16 *)0x0400000e = (*(u16 *)0x0400000e & 0x43) | 0x1d1c;
        *(u16 *)0x0400000e = (*(u16 *)0x0400000e & ~3) | 3;
        *(u16 *)0x0400000a &= ~3;
        *(u16 *)(base + 0xadec) = 0;
        *(u16 *)(base + 0xadee) = 0x20;
        *(u32 *)0x0400001c = 0;
        *(u32 *)0x04000018 = 0;
        *(u32 *)0x04000014 = (*(u16 *)(base + 0xadec) & 0x1ff)
                           | ((u32)(*(u16 *)(base + 0xadee) << 16) & 0x01ff0000);
        break;
    case 1:
        break;
    }
    return 0;
}
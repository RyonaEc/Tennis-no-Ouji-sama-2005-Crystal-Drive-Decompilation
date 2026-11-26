#include "types.h"

extern void func_0202a768(u8 *p);

u32 func_0202ac78(u8 *p, u8 *dst)
{
    s32 i;

    if (p == 0 || dst == 0) {
        return 1;
    }
    if (*(u32 *)(p + 0x64) != 0) {
        return *(u32 *)(p + 0x64);
    }
    if (*(u32 *)(p + 0x60) == 0) {
        func_0202a768(p);
        for (i = 0; i < 0x40; i++) {
            *(u8 *)(p + 0x20 + i) = 0;
        }
        *(u32 *)(p + 0x14) = 0;
        *(u32 *)(p + 0x18) = 0;
        *(u32 *)(p + 0x60) = 1;
    }
    for (i = 0; i < 0x14; i++) {
        *(u8 *)(dst + i) = (u8)(((u32 *)p)[i >> 2] >> ((3 - (i & 3)) * 8));
    }
    return 0;
}
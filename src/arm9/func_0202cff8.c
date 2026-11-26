#include "types.h"

extern u32 data_0204aec0[];
extern u32 func_02025118(u32 a0);

void func_0202cff8(u32 a0, u32 a1, u32 a2)
{
    u8 *b;
    u8 *f;

    if (a0 != 0xb)
        return;
    if (a2 == 0)
        return;
    b = (u8 *)data_0204aec0;
    f = b + 0x34;
    *(u32 *)f &= ~0x20;
    func_02025118(*(u32 *)(b + 0xd0));
}
#include "types.h"

extern void func_020270d8(u32, u32);

void func_0201ca9c(u8 *o, s32 i, u32 v)
{
    u8 *p;

    if (i > *(s32 *)(o + 0x48) - 1)
        return;
    p = o + i;
    func_020270d8(1 << p[0x4c], v);
}
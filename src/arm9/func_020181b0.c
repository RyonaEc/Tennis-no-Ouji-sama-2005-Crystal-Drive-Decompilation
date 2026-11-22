#include "types.h"

extern u8 *func_020183ac(void *, u32, u32);

s32 func_020181b0(u8 *a0, u32 a1)
{
    u32 t;
    u8 *b;
    u8 *p;

    b = a0 + 0x24;
    t = *(u32 *)b;
    p = func_020183ac(b, 0x10, 4);
    if (p == 0)
        return 0;
    *(u32 *)p = a1;
    *(u32 *)(p + 4) = t;
    *(u32 *)(p + 8) = *(u32 *)(b + 4);
    *(u32 *)(p + 0xc) = *(u32 *)(b + 8);
    *(u32 *)(b + 8) = (u32)p;
    return 1;
}
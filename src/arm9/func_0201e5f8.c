#include "types.h"

extern u8 *func_0201d554(u32 a0);
extern s32 func_0201e2ac(u32 a0, s32 a1, s32 a2, s32 a3, u32 a4, u32 a5);

s32 func_0201e5f8(u32 a0, s32 a1, s32 a2, s32 a3, u32 a4)
{
    u8 *p = func_0201d554(a4);

    if (p == 0)
        return 0;
    if (a3 < 0)
        a3 = *(u8 *)(p + 8);
    if (a2 < 0)
        a2 = *(u16 *)(p + 4);
    if (a1 < 0)
        a1 = *(u8 *)(p + 9);
    return func_0201e2ac(a0, a1, a2, a3, (u32)p, a4);
}
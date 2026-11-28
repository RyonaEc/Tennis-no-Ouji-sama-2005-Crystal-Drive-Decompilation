#include "types.h"

extern u32 func_0201d554(s32 idx);
extern u32 func_0201d48c(s32 idx);
extern s32 func_0201dd70(u32 a0, u32 a1);
extern u32 func_0201d428(s32 idx);
extern s32 func_0201dd10(u32 a0, u32 a1);
extern void func_02027c9c(void *a0, u32 a1, void *node);
extern s32 func_0201de30(u32 a0, u32 a1);

s32 func_0201e014(s32 a0, u32 a1)
{
    u8 *p = (u8 *)func_0201d554(a0);
    u8 *q;
    s32 r;
    s32 i;

    if (!p)
        return 0;
    q = (u8 *)func_0201d48c(*(u16 *)(p + 4));
    if (!q)
        return 0;
    r = func_0201dd70(*(u32 *)q, a1);
    if (!r)
        return 0;
    for (i = 0; i < 4; i++) {
        u32 t = ((u16 *)(q + 4))[i];
        u8 *w;
        s32 n;

        if (t == 0xffff)
            continue;
        w = (u8 *)func_0201d428(t);
        if (!w)
            return 0;
        n = func_0201dd10(*(u32 *)w, a1);
        if (!n)
            return 0;
        func_02027c9c((void *)r, i, (void *)n);
    }
    return func_0201de30(*(u32 *)p, a1) != 0;
}
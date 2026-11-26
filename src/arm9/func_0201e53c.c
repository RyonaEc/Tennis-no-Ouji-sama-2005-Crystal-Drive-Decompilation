#include "types.h"

extern u32 func_0201d4f0(s32 idx);
extern u32 func_0201d1e0(u32 a0);
extern u8 *func_0201fbc4(u8 *a0, s32 i);
extern u32 func_0201e174(u32 a0, s32 a1, s32 a2, s32 a3, u8 *a4, u32 a5, s32 a6,
                         s32 a7);

u32 func_0201e53c(u32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5)
{
    u32 *p;
    u32 e;
    u8 *r;

    p = (u32 *)func_0201d4f0(a4);
    if (p == 0) {
        return 0;
    }
    e = func_0201d1e0(*p);
    if (e == 0) {
        return 0;
    }
    r = func_0201fbc4((u8 *)e, a5);
    if (r == 0) {
        return 0;
    }
    if (a3 < 0) {
        a3 = *(u8 *)(r + 8);
    }
    if (a2 < 0) {
        a2 = *(u16 *)(r + 4);
    }
    if (a1 < 0) {
        a1 = *(u8 *)(r + 9);
    }
    return func_0201e174(a0, a1, a2, a3, r, e, a4, a5);
}
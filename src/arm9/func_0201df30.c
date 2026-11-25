#include "types.h"

typedef struct {
    u32 f;
    u16 w[4];
} Slot;

extern u32 func_0201d48c(s32 idx);
extern s32 func_0201dd70(u32 a0, u32 a1);
extern u32 func_0201d428(s32 idx);
extern s32 func_0201dd10(u32 a0, u32 a1);
extern void func_02027c9c(void *a0, u32 a1, void *a2);

u32 func_0201df30(s32 a0, u32 a1)
{
    Slot *p = (Slot *)func_0201d48c(a0);
    u8 *q;
    u8 *r;
    u8 *s;
    s32 i;

    if (p == 0)
        return 0;
    q = (u8 *)func_0201dd70(p->f, a1);
    if (q == 0)
        return 0;
    for (i = 0; i < 4; i++) {
        if (p->w[i] == 0xffff)
            continue;
        r = (u8 *)func_0201d428(p->w[i]);
        if (r == 0)
            return 0;
        s = (u8 *)func_0201dd10(*(u32 *)r, a1);
        if (s == 0)
            return 0;
        func_02027c9c((void *)q, i, (void *)s);
    }
    return 1;
}
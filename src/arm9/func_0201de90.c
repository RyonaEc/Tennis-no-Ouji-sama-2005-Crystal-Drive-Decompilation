#include "types.h"

extern void *func_0201d28c(u32);
extern void *func_0201da30(u32, void *, u32, u32, u32);
extern u32 func_0201d208(u32, void *, void *, u32);
extern void func_02025c78(void *, void *);

u32 func_0201de90(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4)
{
    u8 *p;
    u8 *q;

    p = func_0201d28c(a0);
    if (!p) return 0;
    q = func_0201da30(a4, p + 0x20, a1, a2, a3);
    if (!q) return 0;
    if (p != (u8 *)func_0201d208(a0, q, p, 0)) return 0;
    func_02025c78(q, p);
    return (u32)q;
}
#include "types.h"

extern u8 data_02047608[];
extern void func_0201c524(u32 *);
extern u32 func_02017784(u8 *, u32);
extern void func_0201c140(u8 *);
extern u8 *func_0201c0c4(s32);
extern void func_0201c1dc(u8 *, u8 *);

u32 func_0201c370(u32 *a0, u32 a1, s32 a2)
{
    u8 *e = data_02047608 + a1 * 0x1c;
    u8 *p;
    u8 *r5;

    if (*a0 != 0) func_0201c524(a0);
    if (*(u16 *)(e + 8) >= *(u32 *)(e + 0x18))
    {
        p = (u8 *)func_02017784(e, 0);
        if (p == 0) return 0;
        if (a2 < *(u8 *)(p + 0x3d)) return 0;
        func_0201c140(p);
    }
    r5 = func_0201c0c4(a2);
    if (r5 == 0) return 0;
    func_0201c1dc(e, r5);
    *(u32 *)r5 = (u32)a0;
    *a0 = (u32)r5;
    return (u32)r5;
}
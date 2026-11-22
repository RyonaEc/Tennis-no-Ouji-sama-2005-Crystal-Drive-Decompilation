#include "types.h"

extern u8 *func_020195dc(u32 a0);
extern void func_020269b8(u32 a0, u8 *a1, u32 a2);

void func_020190f8(u32 a0, u32 a1, u32 a2)
{
    u8 *p = func_020195dc(a0);
    u16 n = (u16)(a2 - a1 + 1);
    u16 i;
    volatile u16 v;

    for (i = 0; i < n; i++, p += 8) {
        v = 0xc0;
        func_020269b8(v, p, 2);
    }
}
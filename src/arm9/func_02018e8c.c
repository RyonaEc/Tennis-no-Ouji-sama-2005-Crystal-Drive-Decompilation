#include "types.h"

extern u32 func_020194a8(u8 *a0, u32 a1);
extern void func_020193fc(u32 a0, u32 a1, u32 a2);

u32 func_02018e8c(u8 *a0, u32 a1)
{
    u16 *c;
    u32 n;

    if (func_020194a8(a0 + 0xa, 1) == 0)
        return 0xfffe;
    n = *(u16 *)(a0 + 0xe);
    func_020193fc(*(u32 *)a0, a1, n);
    c = (u16 *)(a0 + 0xe);
    *c = *c + 1;
    return n;
}
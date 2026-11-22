#include "types.h"

extern void func_0202660c(u32, u32, u32);
extern void func_01ff8504(u32, u32, u32, u32);

void func_02026e2c(u32 a0, u32 a1, u32 a2, u32 a3)
{
    volatile u32 *p;

    func_0202660c(a0, (u32)-1, a2);
    if (a3 == 0)
        return;
    p = (volatile u32 *)(0x040000b0 + (a0 * 3 + 2) * 4);
    while (*p & 0x80000000)
        ;
    func_01ff8504(a0, a1, a2, 0xaf000001);
}
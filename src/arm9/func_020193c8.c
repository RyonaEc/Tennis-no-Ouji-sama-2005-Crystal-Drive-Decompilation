#include "types.h"

extern u16 *func_020195dc(u32, u32);
extern void func_020269b8(u16, u16 *, u32);
void func_020193c8(u32 a0)
{
    u16 *p = func_020195dc(a0, 0);
    volatile u16 n = 0xc0;
    func_020269b8(n, p, 0x400);
}

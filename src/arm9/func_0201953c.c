#include "types.h"

extern void func_020269b8(u16, u16 *, s32);
void func_0201953c(u16 *a0, s32 a1, s32 a2, u32 a3)
{
    volatile u16 x = a3;
    func_020269b8(x, (u16 *)((u8 *)a0 + a1 * 2), (a2 - a1) * 2);
}

#include "types.h"

extern void func_020269b8(u16, u16 *, s32);
void func_02019504(u16 *a0, s32 a1, s32 a2)
{
    volatile u16 n = 0xffff;
    func_020269b8(n, (u16 *)((u8 *)a0 + a1 * 2), (a2 - a1) * 2);
}

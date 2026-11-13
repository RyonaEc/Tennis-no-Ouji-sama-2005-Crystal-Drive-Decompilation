#include "types.h"

extern void func_020048e8(u32, u8 *, u32);
void func_ov002_0204da1c(u8 *a0, u16 a1)
{
    volatile u32 z = 0;
    func_020048e8(z, a0, 8);
    *(u16 *)a0 = a1;
}

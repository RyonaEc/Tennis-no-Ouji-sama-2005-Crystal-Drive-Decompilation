#include "types.h"

extern void func_020048e8(u32, u8 *, u32);
void func_ov004_020527d0(u8 *a0, s32 a1)
{
    volatile u32 z = 0;

    func_020048e8(z, a0, 0x20);
    *(u16 *)(a0 + 6) = a1;
}

#include "types.h"

extern u8 data_020475fc[];
extern void func_0201779c(u32, u32);
extern void func_0201c1dc(u32, u8 *);
extern void func_0201c17c(u8 *);

void func_0201bf74(u8 *a0, u32 a1)
{
    u32 old = *(u32 *)(a0 + 4);

    if (old != 0)
    {
        func_0201779c(old, (u32)a0);
        *(u32 *)(a0 + 4) = 0;
    }
    func_0201779c((u32)data_020475fc, (u32)a0);
    a0[0x3d] = a1;
    if (old != 0) func_0201c1dc(old, a0);
    func_0201c17c(a0);
}
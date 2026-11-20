#include "types.h"

extern u8 data_02047f00[];
extern u32 func_020297bc(u8 *, u32, u32);
extern void func_02029828(u8 *, u32, u32);

s32 func_0201d208(u32 a0, u32 a1, u32 a2, u32 a3)
{
    u8 *d = *(u8 **)data_02047f00;
    u32 *q = *(u32 **)(d + 0x7c);
    u32 *e;
    u32 n;

    if (a0 >= q[2]) return -1;
    e = (u32 *)((u8 *)q + 0xc + a0 * 16);
    n = e[1] - a3;
    if (a2 > n) a2 = n;
    if (func_020297bc(d + 0x30, e[0] + a3, 0) == 0) return -1;
    func_02029828(d + 0x30, a1, a2);
}
#include "types.h"

extern u32 data_ov002_02102070[];
extern void func_020048e8(u32 data, void *destp, u32 size);

void func_ov002_0206d600(s32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u32 *p = (u32 *)(base + 0x1dd20c);
    volatile u32 z;

    p[0] = a0;
    p[1] = 0;
    z = 0;
    func_020048e8(z, p + 2, 0x800);
}
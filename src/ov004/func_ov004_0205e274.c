#include "types.h"

extern u32 data_ov004_020690a0;

extern void func_020051b8(u8 *a0, s32 a1);

void func_ov004_0205e274(void)
{
    u8 *p = (u8 *)data_ov004_020690a0 + 0x11f0;

    s32 *q = (s32 *)(p + 0x180);

    *q = 0;
    func_020051b8(p + 0x184, *q);
    *(u32 *)(p + 4) = 0xffffffff;
}
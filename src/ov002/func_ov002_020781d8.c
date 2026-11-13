#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_020781d8(u32 a0)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x1e6c);
    if (p[5] == a0)
        *(u32 *)p &= ~1;
}

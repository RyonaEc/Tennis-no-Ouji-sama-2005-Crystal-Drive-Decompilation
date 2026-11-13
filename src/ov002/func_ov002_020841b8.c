#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_020841b8(void)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x36ac);
    p[9] = 0;
    *(u32 *)(p + 0xc) = 0;
    *(u32 *)(p + 0x10) = 0;
    p[0x14] = 0;
}

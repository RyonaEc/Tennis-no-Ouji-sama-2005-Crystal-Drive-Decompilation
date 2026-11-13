#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_0204e84c(u32 a0, u32 a1)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x1e34);
    *(u16 *)(p + 0x22) = 0;
    *(u16 *)(p + 0x24) = a0;
    *(u16 *)(p + 0x26) = a1;
}

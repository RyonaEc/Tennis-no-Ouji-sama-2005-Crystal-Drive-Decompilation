#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_0206e70c(u32 a0)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x1e6c);
    p[0x612] = a0;
    *(u32 *)(p + 0x614) = 0x20;
}

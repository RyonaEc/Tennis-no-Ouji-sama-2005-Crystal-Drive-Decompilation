#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
s32 func_ov002_0207a93c(void)
{
    u32 v = *(u32 *)(B + 0x18dc);
    return !(v & 1) || (v & 0x10000000) != 0;
}

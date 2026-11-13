#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_020848d0(void)
{
    return *(u32 *)((*(u32 *)((u8 *)data_ov002_02102070) + 0x3000) + 1756);
}

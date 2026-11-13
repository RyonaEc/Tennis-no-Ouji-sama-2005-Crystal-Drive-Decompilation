#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_02068438(void)
{
    return *(u16 *)((*(u32 *)((u8 *)data_ov002_02102070) + 0x3600) + 66);
}

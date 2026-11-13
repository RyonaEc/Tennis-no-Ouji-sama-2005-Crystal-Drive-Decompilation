#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_0205bf64(void)
{
    return *(s16 *)((*(u32 *)((u8 *)data_ov002_02102070) + 0x1e00) + 92);
}

#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_02060024(void)
{
    return *(u8 *)(*(u32 *)((u8 *)data_ov002_02102070) + 263);
}

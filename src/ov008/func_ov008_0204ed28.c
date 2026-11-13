#include "types.h"

extern u8 data_ov008_0204f440[];

u32 func_ov008_0204ed28(void)
{
    return *(u32 *)(*(u32 *)((u8 *)data_ov008_0204f440) + 268);
}

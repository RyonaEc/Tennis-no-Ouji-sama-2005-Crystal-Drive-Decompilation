#include "types.h"

extern u8 data_ov002_02102070[];
s32 func_ov002_02088ad0(void)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x4bc4);
    if (*(s32 *)(p + 0x364) > 0) return -1;
    return *(u8 *)(p + 0x35e) == 5;
}

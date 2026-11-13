#include "types.h"

extern u8 data_ov002_02102070[];
u32 func_ov002_0204e004(void)
{
    return *(u8 *)(*(u8 **)data_ov002_02102070 + 0xae) == 5;
}

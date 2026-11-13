#include "types.h"

extern u8 data_ov002_02102070[];
u32 func_ov002_02079790(u32 a0)
{
    return *(u8 *)(*(u8 **)data_ov002_02102070 + a0 + 0x3000 + 0x6a8) == 1;
}

#include "types.h"

extern u8 data_ov002_02102070[];
u32 func_ov002_02085fd4(u32 a0)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    return (*(u8 *)(b + 0x47d4) & a0) != 0;
}

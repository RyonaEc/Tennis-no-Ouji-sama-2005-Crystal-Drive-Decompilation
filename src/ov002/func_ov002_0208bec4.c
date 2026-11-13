#include "types.h"

extern u8 data_ov002_02102070[];
u32 func_ov002_0208bec4(void)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    return (*(u32 *)(b + 0x4000 + 0xbc8) & 3) != 0;
}

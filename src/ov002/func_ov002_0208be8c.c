#include "types.h"

extern u8 data_ov002_02102070[];
u32 func_ov002_0208be8c(u32 a0)
{
    u8 *q = (u8 *)(*(u8 **)data_ov002_02102070 + 0x4bc4);
    u32 v = *(u32 *)(q + 4);
    u32 m = 1 << q[a0];
    return (v & m) != 0;
}

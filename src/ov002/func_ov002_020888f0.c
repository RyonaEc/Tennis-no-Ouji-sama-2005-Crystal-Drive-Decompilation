#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_ov002_0206004c(u32);
void func_ov002_020888f0(u32 a0, u32 a1)
{
    u8 *b = (u8 *)(*(u8 **)data_ov002_02102070 + 0x4bc4);
    if (func_ov002_0206004c(a0) != a0) return;
    if (*(u32 *)(b + 4) & 4)
    {
        *(u8 *)(b + a1 + 0x345) = 1;
        *(u8 *)(b + a1 + 0x343) = 0;
    }
}

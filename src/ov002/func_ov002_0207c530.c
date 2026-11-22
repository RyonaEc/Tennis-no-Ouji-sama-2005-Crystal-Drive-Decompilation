#include "types.h"

extern u8 data_ov002_02102070;
extern s32 func_ov002_0207afbc(u32, u32, u32);

void func_ov002_0207c530(u8 *obj)
{
    u32 *b = (u32 *)*(u32 *)&data_ov002_02102070;
    u32 *p;

    if (func_ov002_0207afbc(obj[0], obj[0xd1], 1) != 1)
        return;

    p = (u32 *)((u8 *)b + 0x38);
    *p = *p & ~1;
    *p = *p | 2;
}
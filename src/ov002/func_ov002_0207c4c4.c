#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_0207c4c4(void)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    s32 i;
    for (i = 0; i < 2; i++)
    {
        *(u8 *)(b + i + 0x3f) = 0;
        *(u8 *)(b + i + 0x3d) = 0;
        *(u8 *)(b + i + 0x43) = 0;
    }
}

#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_0207c498(void)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    s32 i;
    for (i = 0; i < 2; i++) {
        *(u8 *)(base + i + 67) = 1;
    }
}

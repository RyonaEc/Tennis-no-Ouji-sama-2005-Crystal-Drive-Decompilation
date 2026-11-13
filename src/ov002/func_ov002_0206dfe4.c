#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_0206dfe4(u32 a0)
{
    u8 *p = B + 0x1e6c;
    if (*(u8 *)(p + 0x618) != a0) return;
    if (*(u8 *)(p + 0x619) != 0)
    {
        *(u8 *)(p + 0x619) = 3;
        *(u32 *)(p + 0x61c) = 4;
    }
}

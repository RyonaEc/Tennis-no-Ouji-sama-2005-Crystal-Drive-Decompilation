#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_02088118(u32 a0, u32 a1)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x1fe);
    s32 v = p[a0];
    if (a1 & 0x200)
    {
        v--;
        if (v < 0) v = 2;
    }
    else if (a1 & 0x100)
    {
        v++;
        if (v > 2) v = 0;
    }
    p[a0] = v;
}

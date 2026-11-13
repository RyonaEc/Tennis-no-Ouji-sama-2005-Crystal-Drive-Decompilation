#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_02089f2c(void)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x4bc4);
    u8 t, u;
    t = p[0x34f];
    p[0x34f] = p[0x350];
    p[0x350] = t;
    u = p[0x351];
    p[0x351] = p[0x352];
    p[0x352] = u;
}

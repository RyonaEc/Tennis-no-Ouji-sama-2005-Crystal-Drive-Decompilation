#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_0207be74(void)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 t1 = p[0x3f];
    u8 t2 = p[0x3d];
    p[0x3f] = p[0x40];
    p[0x3d] = p[0x3e];
    p[0x40] = t1;
    p[0x3e] = t2;
}

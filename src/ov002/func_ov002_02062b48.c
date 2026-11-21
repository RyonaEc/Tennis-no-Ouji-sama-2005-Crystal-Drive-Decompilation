#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

void func_ov002_02062b48(void)
{
    u32 *p = (u32 *)B;
    u32 v;

    p += 0xa;
    v = p[0];
    p[0] = v & ~4;
}
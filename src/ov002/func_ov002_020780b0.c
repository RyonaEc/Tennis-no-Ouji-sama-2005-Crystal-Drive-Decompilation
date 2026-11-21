#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

void func_ov002_020780b0(void)
{
    u8 *p = B + 0x1e6c;

    *(u32 *)p |= 2;
    p[0x58] = 0;
    p[0x59] = 0xa;
    p[0x5a] = 0;
    *(u32 *)p &= ~4;
}
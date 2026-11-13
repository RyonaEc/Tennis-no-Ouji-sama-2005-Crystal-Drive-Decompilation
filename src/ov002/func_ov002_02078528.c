#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_02078528(u32 a0)
{
    u8 *b = B;
    u8 *p;
    u8 v;
    if (*(u32 *)(b + 0x1000 + 0x8f4) == 0) return;
    p = b + 0x24a;
    v = *(u8 *)(p + a0);
    if (v < 0xff)
        *(u8 *)(p + a0) = v + 1;
}

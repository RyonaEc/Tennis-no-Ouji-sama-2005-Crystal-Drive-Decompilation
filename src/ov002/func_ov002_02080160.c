#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_02080160(u32 a0, s32 a1)
{
    u8 *t = B + 0x3654;
    if (a1 > 0x640000) a1 = 0x640000;
    if (a1 < 0) a1 = 0;
    *(u32 *)(t + a0 * 4 + 0x28) = a1;
}

#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_02068c4c(u32 a0)
{
    u32 i = *(u8 *)(B + a0 * 0x158 + 0x2e6);
    *(u8 *)(B + (i >> 1) + 0x2000 + 0x4ec) = 0;
}

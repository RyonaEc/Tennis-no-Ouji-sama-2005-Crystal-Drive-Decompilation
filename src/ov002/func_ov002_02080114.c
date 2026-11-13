#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern void func_ov002_0207fff0(u32, u32);
void func_ov002_02080114(u32 a0, s32 a1)
{
    u8 *b = B;
    if (*(u8 *)(b + a0 + 0x3d) != 0) return;
    if (*(u8 *)(b + 0x8c) == 1)
        a1 = a1 / 2;
    func_ov002_0207fff0(a0, a1);
}

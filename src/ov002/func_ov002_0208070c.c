#include "types.h"

extern u8 data_ov002_02102070[];
extern s32 func_ov002_0207fdd0(u32);
extern void func_ov002_0208102c(u32, u32);

void func_ov002_0208070c(u32 a0)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *q = p + a0 * 0x158;
    u8 v;

    v = ((u32)*(u8 *)(q + 0x2e6) >> 1) & 0xff;
    if (*(u8 *)(p + 0x3670 + v) != 1)
        return;
    if (func_ov002_0207fdd0(v == 0) != 4)
        return;
    func_ov002_0208102c(v, 0x5a0000);
}

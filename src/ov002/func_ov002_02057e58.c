#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_0204d2b4(u32 a0, u32 a1, u32 a2);
extern void func_ov002_0204c7e8(u8 *a0, u32 a1);

void func_ov002_02057e58(u32 a0, u32 a1)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    u8 *e = base + 0x2e4 + a0 * 0x158;
    u32 idx = e[2];
    u8 *r = base + 0x274 + idx * 0x1c;

    if (idx > 1)
        a1 = (a1 == 0x55) ? 0x56 : 0x55;
    func_ov002_0204d2b4(idx, e[1], a1 & 0xff);
    func_ov002_0204c7e8(r, 1);
}
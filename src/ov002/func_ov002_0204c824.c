#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

extern u8 data_ov002_02100a94[];
extern void func_ov002_0204f3cc(u32, u32, u32);

void func_ov002_0204c824(u32 a0, u32 a1)
{
    u8 *p = B + 0x274 + a0 * 0x1c;
    u32 *q = (u32 *)(p + 0x18);
    u32 v = *q;

    *q = v & ~0x400;
    func_ov002_0204f3cc(*(u32 *)(data_ov002_02100a94 + a1 * 0x30), a0 << 5, 0x20);
}
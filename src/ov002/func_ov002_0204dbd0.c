#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_02024674(u8 *);
extern void func_02025c94(u32, u32);
void func_ov002_0204dbd0(u32 a0)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    u8 *p = b + 0x1a89a4 + a0 * 0x10018;
    *(u32 *)(p + 0x14) = func_02024674(p);
    func_02025c94(*(u32 *)(p + 8), *(u32 *)(p + 0x14));
}

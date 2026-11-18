#include "types.h"

extern u8 data_ov008_0204f440[];
extern s32 func_02024674(s32);
extern void func_02025c94(s32, s32);

void func_ov008_0204c894(s32 idx)
{
    u8 *base = *(u8 **)data_ov008_0204f440;
    u8 *e = base + 0x2576c + idx * 0x10018;

    *(u32 *)(e + 0x14) = func_02024674((s32)e);
    func_02025c94(*(u32 *)(e + 8), *(u32 *)(e + 0x14));
}

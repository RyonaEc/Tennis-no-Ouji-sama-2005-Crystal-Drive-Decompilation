#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_02080114(u32 a0, s32 a1);

void func_ov002_0207fa7c(u32 a0, u32 a1)
{
    if (*(u32 *)(*(u32 *)data_ov002_02102070 + 0x18ec) & 0x20) {
        u8 v = (a0 == 0);

        func_ov002_02080114(v, 0x1e0000);
    }
    if (!(a1 & 0x800000))
        return;
    func_ov002_02080114(a0 & 0xff, 0xa0000);
}
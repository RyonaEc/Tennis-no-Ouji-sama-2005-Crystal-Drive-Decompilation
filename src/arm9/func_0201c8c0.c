#include "types.h"

extern u32 func_020271d4(u32, u32, u32, u32);
extern u32 func_020274c4(u32);
extern void func_02027580(u32);
extern void func_020274f0(u32);

void func_0201c8c0(u8 *a0)
{
    u32 x;
    u32 s;

    if (((s32)(*(u32 *)(a0 + 0x24) << 0x1e) >> 0x1f) == 0) return;
    x = func_020271d4(*(u32 *)(a0 + 0x44), 0, 1 << *(u32 *)(a0 + 0x40), 0);
    s = func_020274c4(x);
    func_02027580(1);
    func_020274f0(s);
}
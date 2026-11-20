#include "types.h"

extern u8 data_02047eb0[];
extern u32 func_020271d4(u32, u32, u32, u32);
extern u32 func_020274c4(u32);
extern void func_02027580(u32);
extern void func_020274f0(u32);

void func_0201cf74(void)
{
    u8 *d = data_02047eb0;
    u32 s;
    s32 sh;

    if (*(u32 *)d == 0) return;
    sh = *(s32 *)(d + 0x28);
    s = func_020274c4(func_020271d4(*(u32 *)(d + 0x20), *(u32 *)(d + 0x24),
        (sh >= 0) ? (1u << sh) : 0, 0));
    func_02027580(1);
    func_020274f0(s);
}
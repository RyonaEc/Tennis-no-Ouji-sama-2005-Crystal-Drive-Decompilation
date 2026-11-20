#include "types.h"

extern u8 data_02047eb0[];
extern void func_020048e8(u32, u32, u32);
extern void func_02025c94(u32, u32);
extern void func_02027244(u32, u32, u32, u32);

void func_0201ced4(void)
{
    u8 *p = data_02047eb0;
    s32 sh;
    volatile u32 z1;
    volatile u32 z2;

    if (*(u32 *)p == 0) return;
    *(u32 *)(p + 0x18) = 0;
    z1 = 0;
    func_020048e8(z1, *(u32 *)(p + 0xc), *(u32 *)(p + 0x14));
    z2 = 0;
    func_020048e8(z2, *(u32 *)(p + 0x10), *(u32 *)(p + 0x14));
    func_02025c94(*(u32 *)(p + 0xc), *(u32 *)(p + 0x14));
    func_02025c94(*(u32 *)(p + 0x10), *(u32 *)(p + 0x14));
    sh = *(s32 *)(p + 0x28);
    func_02027244(*(u32 *)(p + 0x20), *(u32 *)(p + 0x24), sh >= 0 ? 1u << sh : 0, 0);
}
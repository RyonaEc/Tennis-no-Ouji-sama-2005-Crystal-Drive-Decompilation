#include "types.h"

extern u32 func_0202e2ec(void);
extern s32 func_0202e1cc(s32, s32, s32);
extern void func_02025c5c(u32, s32);

s32 func_0202e7ec(void)
{
    u8 *h = (u8 *)func_0202e2ec();
    s32 f;
    u16 v;

    if (func_0202e1cc(2, 7, 8))
        return 0;
    func_02025c5c(*(u32 *)(h + 4) + 0x10, 4);
    if (*(u32 *)(*(u32 *)(h + 4) + 0x10) == 1)
        return 0;
    func_02025c5c(*(u32 *)(h + 4) + 0x184, 2);
    f = (*(u16 *)(*(u32 *)(h + 4) + 0x184) == 0);
    func_02025c5c(*(u32 *)(h + 4) + 0x190, 2);
    v = *(u16 *)(*(u32 *)(h + 4) + 0x190);
    if (f != 1)
        return ((v + 0x51) & ~0x1f) << 1;
    func_02025c5c(*(u32 *)(h + 4) + 0xf4, 2);
    return (((v + 0xc) * *(u16 *)(*(u32 *)(h + 4) + 0xf4) + 0x29) & ~0x1f) << 1;
}
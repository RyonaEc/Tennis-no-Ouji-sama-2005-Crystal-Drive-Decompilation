#include "types.h"

extern u32 func_0202e2ec(void);
extern s32 func_0202e1cc(s32, s32, s32);
extern void func_02025c5c(u32, s32);
extern void func_02025c78(u32, s32);
extern s32 func_0202e2fc(s32, s32, s32, s32, s32, s32, s32, s32, s32);

s32 func_0202f118(s32 a0, s32 a1, s32 a2, s32 a3, u16 b0, u16 b1, u16 b2)
{
    u16 n;
    s32 m = 1;
    s32 f;
    s32 r;
    u8 *h = (u8 *)func_0202e2ec();

    r = func_0202e1cc(2, 9, 0xa);
    if (r != 0)
        return r;
    func_02025c5c(*(u32 *)(h + 4) + 0x18e, 2);
    n = *(u16 *)(*(u32 *)(h + 4) + 0x18e);
    func_02025c5c(*(u32 *)(h + 4) + 0x184, 2);
    f = (*(u16 *)(*(u32 *)(h + 4) + 0x184) == 0);
    if (f == 1) {
        func_02025c5c(*(u32 *)(h + 4) + 0x17e, 2);
        m = *(u16 *)(*(u32 *)(h + 4) + 0x17e);
        func_02025c5c(*(u32 *)(h + 4) + 0x86, 2);
    }
    if (a2 == 0)
        return 6;
    if (m == 0)
        return 7;
    if (a3 + (f ? 4 : 2) > n)
        return 6;
    if (a3 == 0)
        return 6;
    func_02025c78((u32)a2, a3);
    r = func_0202e2fc(0xf, 7, a2, a3, b0, b1, b2, a0, a1);
    if (r == 0)
        r = 2;
    return r;
}
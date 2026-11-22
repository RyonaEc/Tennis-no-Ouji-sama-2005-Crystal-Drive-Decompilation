#include "types.h"

extern u32 func_0202e2ec(void);
extern s32 func_0202e1cc(s32, s32, s32, s32, s32, s32);
extern void func_02025c5c(u32, s32);
extern void func_0202e418(s32, s32);
extern s32 func_0202e2fc(s32, s32, s32);

s32 func_0202ea20(s32 a0, u32 a1)
{
    u8 *h = (u8 *)func_0202e2ec();
    u16 v;
    s32 r;

    r = func_0202e1cc(5, 7, 9, 8, 0xa, 0xb);
    if (r != 0)
        return r;
    v = *(u16 *)(*(u32 *)(h + 4));
    if (v == 7 || v == 9) {
        if (a1 < 1 || a1 > 0xf)
            return 6;
        func_02025c5c(*(u32 *)(h + 4) + 0x17e, 2);
        if ((*(u16 *)(*(u32 *)(h + 4) + 0x17e) & (1 << a1)) == 0)
            return 7;
    } else {
        if (a1 != 0)
            return 6;
    }
    func_0202e418(0xd, a0);
    r = func_0202e2fc(0xd, 1, 1 << a1);
    if (r == 0)
        r = 2;
    return r;
}
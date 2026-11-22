#include "types.h"

extern u32 func_0202e2ec(void);
extern s32 func_0202e1cc(s32, s32, s32);
extern void func_02025c5c(u32, s32);
extern u32 func_0202ffe4(void *, s32, s32, s32, s32);

s32 func_020303a0(void (*cb)(u16 *))
{
    u8 *h = (u8 *)func_0202e2ec();
    s32 r;
    u16 msg[2];

    r = func_0202e1cc(2, 9, 0xa);
    if (r != 0)
        return r;
    func_02025c5c(*(u32 *)(h + 4) + 0x10, 4);
    if (*(u32 *)(*(u32 *)(h + 4) + 0x10) == 0)
        return 3;
    *(u32 *)(h + 0x13c) = 1;
    r = func_0202ffe4(h + 0x140, 8, 0xffff, 2, 1);
    msg[1] = r;
    msg[0] = 0x15;
    cb(msg);
    return 2;
}
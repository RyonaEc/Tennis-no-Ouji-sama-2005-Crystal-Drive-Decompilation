#include "types.h"

extern u32 func_0202e2ec(void);
extern s32 func_0202e1cc(s32, s32, s32);
extern void func_02025c5c(u32, s32);
extern u32 func_0202ff28(void *);

s32 func_0203030c(void (*cb)(u16 *))
{
    u8 *h = (u8 *)func_0202e2ec();
    s32 r;
    u32 v;
    u16 msg[4];

    r = func_0202e1cc(2, 9, 0xa);
    if (r != 0)
        return r;
    func_02025c5c(*(u32 *)(h + 4) + 0x18, 4);
    if (*(u32 *)(*(u32 *)(h + 4) + 0x18) == 0)
        return 3;
    v = func_0202ff28(h + 0x140);
    *(u32 *)(h + 0x13c) = 0;
    msg[1] = v;
    msg[0] = 0x16;
    cb(msg);
    return 2;
}
#include "types.h"

extern u32 func_0202e2ec(void);
extern s32 func_0202e1cc(s32, s32, s32, s32);
extern void func_02025c78(void *, s32);
extern void func_0202e418(s32, s32);
extern s32 func_0202e2fc(s32, s32);

s32 func_0202ec1c(s32 a0, u8 *p)
{
    u8 *q;
    s32 r;

    r = func_0202e1cc(3, 2, 3, 5);
    if (r != 0)
        return r;
    if (p == 0)
        return 6;
    if (*(u32 *)p == 0)
        return 6;
    if (*(u16 *)(p + 4) < 1 || *(u16 *)(p + 4) > 0xe)
        return 6;
    func_02025c78(p, 0x20);
    func_0202e418(0xa, a0);
    q = *(u8 **)((u8 *)func_0202e2ec() + 0xc);
    *(u16 *)q = 0xa;
    *(u16 *)(q + 2) = *(u16 *)(p + 4);
    *(u32 *)(q + 4) = *(u32 *)p;
    *(u16 *)(q + 8) = *(u16 *)(p + 6);
    *(u8 *)(q + 0xa) = p[8];
    *(u8 *)(q + 0xb) = p[9];
    *(u8 *)(q + 0xc) = p[10];
    *(u8 *)(q + 0xd) = p[11];
    *(u8 *)(q + 0xe) = p[12];
    *(u8 *)(q + 0xf) = p[13];
    r = func_0202e2fc(0xa, 0);
    if (r == 0)
        r = 2;
    return r;
}
#include "types.h"

extern s32 func_0202e1cc(s32, s32);
extern void func_0202edbc(void *);
extern void func_0202e418(s32, s32);
extern void func_02025c78(u32, s32);
extern s32 func_0202e2fc(s32, s32, s32);

s32 func_0202ee48(s32 a0, u8 *p)
{
    s32 r;
    s32 a;
    s32 b;

    r = func_0202e1cc(1, 2);
    if (r != 0)
        return r;
    if (p == 0)
        return 6;
    if (*(u16 *)(p + 4) != 0 && *(u32 *)p == 0)
        return 6;
    a = *(u16 *)(p + 0x14);
    if (*(u16 *)(p + 0x34) + (a ? 0x2a : 0) > 0x200 ||
        *(u16 *)(p + 0x36) + (a ? 6 : 0) > 0x200)
        return 6;
    func_0202edbc(p);
    func_0202e418(7, a0);
    func_02025c78((u32)p, 0x40);
    if (*(u16 *)(p + 4) != 0)
        func_02025c78(*(u32 *)p, *(u16 *)(p + 4));
    r = func_0202e2fc(7, 1, (s32)p);
    if (r == 0)
        r = 2;
    return r;
}
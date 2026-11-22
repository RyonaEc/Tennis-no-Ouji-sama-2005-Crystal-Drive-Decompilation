#include "types.h"

extern u32 func_0202e2ec(void);
extern s32 func_0202e1cc(u32, u32);
extern void func_0202e418(u32, u32);
extern s32 func_0202e2fc(u32, u32);

s32 func_020304a8(s32 a0, s32 a1, s32 a2, s32 a3, u16 a4)
{
    void *p = (void *)func_0202e2ec();
    u16 *q;
    s32 r;

    r = func_0202e1cc(1, 2);
    if (r != 0)
        return r;
    func_0202e418(0x1e, a0);
    q = *(u16 **)((u8 *)p + 0xc);
    q[0] = 0x1e;
    q[1] = a1;
    q[2] = a2;
    q[3] = a3;
    q[4] = a4;
    r = func_0202e2fc(0x1e, 0);
    if (r == 0)
        r = 2;
    return r;
}
#include "types.h"

extern s32 func_0202e1cc(u32, u32, u32);
extern void func_0202e418(u32, u32);
extern s32 func_0202e2fc(u32, u32, u32);

s32 func_02030448(s32 a0, s32 a1)
{
    s32 r;

    r = func_0202e1cc(2, 7, 9);
    if (r != 0)
        return r;
    func_0202e418(0x21, a0);
    r = func_0202e2fc(0x21, 1, a1);
    if (r == 0)
        r = 2;
    return r;
}
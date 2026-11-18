#include "types.h"

extern s32 func_0202e1cc(s32, s32);
extern void func_0202e418(s32, s32);
extern s32 func_0202e2fc(s32, s32);

s32 func_0202ed2c(s32 a0)
{
    s32 r = func_0202e1cc(1, 0x7);

    if (r != 0)
        return r;
    func_0202e418(0x9, a0);
    r = func_0202e2fc(0x9, 0);
    if (r == 0)
        r = 2;
    return r;
}
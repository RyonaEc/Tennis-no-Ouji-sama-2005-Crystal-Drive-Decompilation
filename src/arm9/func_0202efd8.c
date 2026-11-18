#include "types.h"

extern s32 func_0202e1cc(s32, s32);
extern void func_0202e418(s32, s32);
extern s32 func_0202e2fc(s32, s32);

s32 func_0202efd8(s32 a0)
{
    s32 r = func_0202e1cc(1, 1);

    if (r != 0)
        return r;
    func_0202e418(5, a0);
    r = func_0202e2fc(5, 0);
    if (r == 0)
        r = 2;
    return r;
}
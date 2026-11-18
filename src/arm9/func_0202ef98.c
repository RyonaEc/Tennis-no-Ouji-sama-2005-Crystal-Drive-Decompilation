#include "types.h"

extern s32 func_0202e274(void);
extern void func_0202e418(s32 a, s32 b);
extern s32 func_0202e2fc(s32 a, s32 b);

s32 func_0202ef98(s32 a0)
{
    s32 r = func_0202e274();

    if (r)
        return r;
    func_0202e418(1, a0);
    r = func_0202e2fc(1, 0);
    if (!r)
        return 2;
    return r;
}

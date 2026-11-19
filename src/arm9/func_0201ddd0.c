#include "types.h"

extern s32 func_0201d1e0(u32 a0);
extern void func_0201d1c4(u32 a0, s32 a1);
extern s32 func_0201d5b8(void);
extern s32 func_0201de90(u32 a0, void *fn, s32 a2, u32 a3, u32 a4);
extern s32 func_0201dc7c(void);

s32 func_0201ddd0(u32 a0, u32 a1)
{
    s32 r = func_0201d1e0(a0);

    if (r == 0) {
        r = func_0201de90(a0, (void *)func_0201dc7c, func_0201d5b8(), a0, a1);
        if (r != 0)
            func_0201d1c4(a0, r);
    }
    return r;
}
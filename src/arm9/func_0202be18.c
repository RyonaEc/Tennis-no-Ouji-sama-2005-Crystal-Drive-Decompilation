#include "types.h"

extern s32 func_0202be54(void *fn, u32 *out);
extern void func_0202c3c0(void);
extern void func_0202c3c8(void);

s32 func_0202be18(void)
{
    u32 v;
    s32 r = func_0202be54((void *)func_0202c3c0, &v);

    if (r)
        return r;
    func_0202c3c8();
    return v;
}

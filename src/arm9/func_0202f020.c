#include "types.h"

extern u32 func_0202e1cc(u32, u32);
extern void func_0202e418(u32, u32);
extern u32 func_0202e2ec(void);
extern u32 func_0202e2fc(u32, u32, u32, u32, u32);

u32 func_0202f020(u32 a0)
{
    u32 r = func_0202e1cc(1, 0);
    u32 *p;

    if (r != 0) return r;
    func_0202e418(3, a0);
    p = (u32 *)func_0202e2ec();
    r = func_0202e2fc(3, 3, p[0], p[1], p[4]);
    return r ? r : 2;
}
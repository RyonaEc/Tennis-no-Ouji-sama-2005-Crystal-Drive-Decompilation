#include "types.h"

extern u32 func_0202e1cc(u32, u32, u32);
extern void func_0202e418(u32, u32);
extern u32 func_0202e2fc(u32, u32, u32);

u32 func_02030448(u32 a0, u32 a1)
{
    u32 r = func_0202e1cc(2, 7, 9);

    if (r != 0) return r;
    func_0202e418(0x21, a0);
    r = func_0202e2fc(0x21, 1, a1);
    return r ? r : 2;
}
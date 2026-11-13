#include "types.h"

extern u32 func_0201c280(u32, u32);
extern u32 func_0201daa0(u32);

u32 func_0201e104(u32 a0, u32 a1)
{
    u32 t0, t1;
    t0 = func_0201c280(a0, a1);
    if (t0 != 0x0) {
        t1 = func_0201daa0(t0);
    }
    return t0;
}

#include "types.h"

extern u32 func_02028ea8(u32);
extern u32 func_02028fe4(u32);

u32 func_020184d0(u32 a0)
{
    u32 t0, t1;
    t0 = func_02028ea8(a0);
    if (t0 == 0x0) return 0x0;
    t1 = func_02028fe4(a0);
    return 0x1;
}

#include "types.h"

extern u32 func_0202f020(u32);
extern u32 func_020176f0(u32);
extern u8 func_02017298[];

u32 func_020172cc(void)
{
    u32 t0, t1;
    t0 = func_0202f020((u32)func_02017298);
    if (t0 == 0x2) return 0x1;
    t1 = func_020176f0(t0);
    return 0x0;
}

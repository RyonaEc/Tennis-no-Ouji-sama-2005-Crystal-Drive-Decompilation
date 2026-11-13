#include "types.h"

extern u32 func_02017710(u32);
extern u32 func_0202f08c(u32);
extern u32 func_020176f0(u32);
extern u8 func_02016a18[];

u32 func_02016a64(void)
{
    u32 t0, t1, t2;
    t0 = func_02017710(0x3);
    t1 = func_0202f08c((u32)func_02016a18);
    if (t1 == 0x2) return 0x1;
    t2 = func_020176f0(t1);
    return 0x0;
}

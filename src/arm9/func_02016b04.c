#include "types.h"

extern u32 func_0203030c(u32);
extern u32 func_020176f0(u32);
extern u32 func_02017710(u32);
extern u32 func_02017544(u32);
extern u8 func_02016aa8[];

u32 func_02016b04(void)
{
    u32 t0, t1, t2, t3;
    t0 = func_0203030c((u32)func_02016aa8);
    if (t0 == 0x0) return 0x1;
    t1 = func_020176f0(t0);
    t2 = func_02017710(0x7);
    t3 = func_02017544(t2);
    return 0x0;
}

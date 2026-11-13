#include "types.h"

extern u32 func_02017710(u32);
extern u32 func_0202ea20(u32, u32);
extern u32 func_020176f0(u32);
extern u32 func_02017544(u32);
extern u8 func_0201641c[];

u32 func_02016450(void)
{
    u32 t0, t1, t2, t3;
    t0 = func_02017710(0x3);
    t1 = func_0202ea20((u32)func_0201641c, 0x0);
    if (t1 == 0x0) return 0x1;
    t2 = func_020176f0(t1);
    t3 = func_02017544(t2);
    return 0x0;
}

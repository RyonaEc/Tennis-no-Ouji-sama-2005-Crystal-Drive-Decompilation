#include "types.h"

extern u32 func_02030448(u32, u32);
extern u32 func_020176f0(u32);
extern u32 func_02017710(u32);
extern u8 func_02015f6c[];

u32 func_02015fa0(void)
{
    u32 t0, t1, t2;
    t0 = func_02030448((u32)func_02015f6c, 0x0);
    if (t0 == 0x2) return 0x1;
    t1 = func_020176f0(t0);
    t2 = func_02017710(0x7);
    return 0x0;
}

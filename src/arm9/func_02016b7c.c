#include "types.h"

extern u32 func_02017710(u32);
extern u32 func_020303a0(u32, u32);
extern u32 func_020176f0(u32);
extern u32 func_02017710(u32);
extern u32 func_02017544(u32);
extern u8 data_02042b80[];
extern u8 func_02016b4c[];

u32 func_02016b7c(void)
{
    u32 t0, t1, t2, t3, t4;
    t0 = func_02017710(0x6);
    t1 = func_020303a0((u32)func_02016b4c, (u32)data_02042b80);
    if (t1 == 0x2) return 0x1;
    t2 = func_020176f0(t1);
    t3 = func_02017710(0x7);
    t4 = func_02017544(t3);
    return 0x0;
}

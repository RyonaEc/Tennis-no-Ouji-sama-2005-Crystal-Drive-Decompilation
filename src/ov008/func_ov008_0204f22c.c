#include "types.h"

extern u32 func_0203447c(u32, u32, u32);
extern u32 func_02029768(u32);
extern u32 func_ov008_0204f0a4(u32);
extern u32 func_ov008_0204ef88(u32);
extern u8 data_ov008_0204f414[];
extern u8 data_ov008_0204f440[];

u32 func_ov008_0204f22c(void)
{
    u32 t0 = func_0203447c((*(u32 *)((u8 *)data_ov008_0204f440) + 0x1114), 0x0, 0x23440);
    u32 t1 = func_02029768((u32)data_ov008_0204f414);
    u32 t2 = func_ov008_0204f0a4(t1);
    u32 t3 = func_ov008_0204ef88(t2);
    return t3;
}

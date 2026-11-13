#include "types.h"

extern u32 func_0203447c(u32, u32, u32);
extern u32 func_ov002_0208ea98(u32);
extern u32 func_ov002_0208e198(u32);
extern u32 func_ov002_0208de3c(u32);
extern u32 func_ov002_0208d92c(u32);
extern u8 data_ov002_02102070[];

u32 func_ov002_0208f09c(void)
{
    u32 t0 = func_0203447c((*(u32 *)((u8 *)data_ov002_02102070) + 0x4f4c), 0x0, 0x1a115c);
    u32 t1 = func_ov002_0208ea98(t0);
    u32 t2 = func_ov002_0208e198(t1);
    u32 t3 = func_ov002_0208de3c(t2);
    u32 t4 = func_ov002_0208d92c(t3);
    return t4;
}

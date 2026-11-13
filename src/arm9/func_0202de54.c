#include "types.h"

extern u32 func_02027d68(void);
extern u32 func_02027f74(u32, u32);
extern u8 data_0204b6e4[];
extern u8 func_0202dde8[];

u32 func_0202de54(void)
{
    u32 t0, t1;
    t0 = func_02027d68();
    t1 = func_02027f74(0xe, (u32)(u32)func_0202dde8);
    *(u32 *)(data_0204b6e4) = 0x0;
    return (u32)data_0204b6e4;
}

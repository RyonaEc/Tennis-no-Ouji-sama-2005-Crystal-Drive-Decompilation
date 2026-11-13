#include "types.h"

extern u32 func_0202d080(u32);
extern u8 data_0204b4a0[];
extern u8 data_0204b4a4[];

void func_0202d04c(u32 a0)
{
    u32 t0 = func_0202d080(a0);
    *(u32 *)((u8 *)data_0204b4a0) = t0;
    *(u32 *)((u8 *)data_0204b4a4) = *(u16 *)(t0 + 14);
}

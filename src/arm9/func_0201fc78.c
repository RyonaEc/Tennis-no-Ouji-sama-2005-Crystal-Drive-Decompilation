#include "types.h"

extern u32 func_0201fc3c(u32);

void func_0201fc78(u32 a0, u32 a1, u32 a2)
{
    u32 t0 = func_0201fc3c(a0);
    *(u32 *)((u8 *)a0) = t0;
    *(u32 *)((u8 *)a0 + 4) = a1;
    *(u32 *)((u8 *)a0 + 12) = a2;
    *(u32 *)((u8 *)a0 + 8) = 0x0;
}

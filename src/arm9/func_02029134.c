#include "types.h"

extern u32 func_02026af4(u32, u32, u32);

void func_02029134(u32 a0)
{
    u32 t0 = func_02026af4(a0, 0x0, 0x50);
    *(u16 *)((u8 *)a0 + 12) = 0x0;
    *(u16 *)((u8 *)a0 + 14) = 0x0;
}

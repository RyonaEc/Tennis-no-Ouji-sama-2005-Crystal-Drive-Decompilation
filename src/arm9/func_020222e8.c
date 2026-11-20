#include "types.h"

void func_020222e8(u8 **a0, u32 a1, u32 a2, u32 a3)
{
    *(u32 *)((u8 *)a0 + 0x10) = 0;
    *(u8 *)*a0 = a1;
    *(u32 *)a0[1] = a2;
    *(u32 *)(a0[1] + 4) = a3;
}
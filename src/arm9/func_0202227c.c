#include "types.h"

extern u32 func_02026ac8(u32, u32, u32, u32);

u32 func_0202227c(u32 a0, u32 a1)
{
    *(u32 *)((u8 *)a0 + 16) = 0x0;
    *(u8 *)(*(u32 *)((u8 *)a0)) = 0x16;
    return func_02026ac8(a1, *(u32 *)((u8 *)a0 + 4), 0x16, a0);
}

#include "types.h"

extern u32 func_02026aa4(u32, u32, u32, u32);

u32 func_02022250(u32 a0, u32 a1)
{
    *(u32 *)((u8 *)a0 + 16) = 0x0;
    *(u8 *)(*(u32 *)((u8 *)a0)) = 0x17;
    return func_02026aa4(a1, *(u32 *)((u8 *)a0 + 4), 0x17, a0);
}

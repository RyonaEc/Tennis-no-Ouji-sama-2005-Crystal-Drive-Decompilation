#include "types.h"

extern u32 func_02026a88(u32, u32, u32, u32);

u32 func_02022224(u32 a0, u32 a1)
{
    *(u32 *)((u8 *)a0 + 16) = 0x0;
    *(u8 *)(*(u32 *)((u8 *)a0)) = 0x1a;
    return func_02026a88(a1, *(u32 *)((u8 *)a0 + 4), 0x1a, a0);
}

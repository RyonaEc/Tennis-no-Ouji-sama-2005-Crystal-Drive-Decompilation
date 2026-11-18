#include "types.h"

void func_020222a8(u8 *ctx, u8 a1, u32 a2, u32 a3, u32 a4)
{
    *(u32 *)(ctx + 0x10) = 0;
    *(u8 *)(*(u8 **)ctx) = a1;
    *(u32 *)(*(u8 **)(ctx + 4)) = a2;
    *(u32 *)(*(u8 **)(ctx + 4) + 4) = a3;
    *(u32 *)(*(u8 **)(ctx + 4) + 8) = a4;
}
#include "types.h"



u32 func_ov004_0205289c(u32 a0, u32 a1, u32 a2)
{
    *(u32 *)((u8 *)a0 + 116) = (a1 << 0xc);
    *(u32 *)((u8 *)a0 + 120) = (a2 << 0xc);
    return a0;
}

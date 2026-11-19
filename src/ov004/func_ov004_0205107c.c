#include "types.h"

extern void func_ov004_020510a0(void);

u32 func_ov004_0205107c(u8 *p)
{
    if (*(u32 *)(p + 0x98) == (u32)func_ov004_020510a0)
    {
        *(u16 *)(p + 0xa4) = 1;
        return 1;
    }
    return 0;
}
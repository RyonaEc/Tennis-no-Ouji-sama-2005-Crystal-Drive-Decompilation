#include "types.h"

extern u32 func_0202474c(u32, u32);

u32 func_0202234c(u32 a0)
{
    *(u32 *)(0x4000400) = 0x17;
    return func_0202474c(a0, 0x4000400);
}

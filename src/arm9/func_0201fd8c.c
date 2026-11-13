#include "types.h"

u32 func_0201fd8c(void)
{
    while (*(volatile u16 *)0x04000280 & 0x8000) {
    }
    return (u32)((*(volatile u64 *)0x040002a0 + 0x80000) >> 0x14);
}

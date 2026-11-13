#include "types.h"

u64 func_0201fdc8(void)
{
    while (*(volatile u16 *)0x04000280 & 0x8000) {
    }
    return *(volatile u64 *)0x040002a0;
}

#include "types.h"

void func_02026710(u32 a0)
{
    volatile u32 *p = (volatile u32 *)(0x040000b0 + (a0 * 3 + 2) * 4);
    while (*p & 0x80000000) {
    }
}

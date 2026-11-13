#include "types.h"

void func_02028ce0(u8 *p, u32 a1, u32 a2)
{
    if (a2 == 0) {
        a1 = 0;
    } else if (a1 == 0) {
        a2 = 0;
    }
    *(u32 *)(p + 0x48) = a1;
    *(u32 *)(p + 0x4c) = a2;
}

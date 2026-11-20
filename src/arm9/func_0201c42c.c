#include "types.h"

void func_0201c42c(u8 **a0, u32 a1, u32 a2)
{
    if (*a0 == 0) return;
    *(u16 *)(*a0 + 0x34) = 2;
    *(u16 *)(*a0 + 0x38) = a1;
    *(u16 *)(*a0 + 0x3a) = a2;
}
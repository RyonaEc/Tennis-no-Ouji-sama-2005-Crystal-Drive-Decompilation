#include "types.h"

void func_ov002_0204c808(u8 *a0, u16 a1)
{
    if (*(u16 *)(a0 + 4) >= a1 + 1)
    {
        *(u16 *)(a0 + 4) = a1;
        *(u16 *)(a0 + 6) = 0;
    }
}

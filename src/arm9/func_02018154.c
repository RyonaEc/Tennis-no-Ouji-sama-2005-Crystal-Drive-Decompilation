#include "types.h"

u32 func_02018154(u8 *a0, u32 a1)
{
    u8 *n;
    u8 *h = a0 + 0x24;

    n = *(u8 **)(h + 8);
    if (a1 != 0 && n != 0)
    {
        do
        {
            if (*(u32 *)n == a1) break;
            n = *(u8 **)(n + 0xc);
        } while (n != 0);
    }
    if (n == 0) return 0;
    *(u32 *)(h + 0) = *(u32 *)(n + 4);
    *(u32 *)(h + 4) = *(u32 *)(n + 8);
    *(u32 *)(h + 8) = *(u32 *)(n + 0xc);
    return 1;
}
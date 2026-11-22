#include "types.h"

extern s32 func_0202f404(u32 a0, u32 a1, u8 *a2, u32 a3);

s32 func_0202f448(u32 a0, u8 *a1, u32 a2)
{
    if (a0 == 0)
        return 0;
    if (a1 == 0)
        return 0;
    if ((*(u16 *)(a1 + 2) & (1 << a2)) == 0)
        return 0;
    return func_0202f404(a0, *(u16 *)a1, a1 + 4, a2);
}
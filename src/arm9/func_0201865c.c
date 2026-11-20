#include "types.h"

u32 func_0201865c(u8 *o)
{
    if (*(u32 *)o != 0x4352414e)
        return 0;
    if (*(u16 *)(o + 4) != 0xfffe)
        return 0;
    return *(u16 *)(o + 6) == 0x100;
}
#include "types.h"

u32 func_020187f8(u8 *p)
{
    return *(u32 *)p + (*(u16 *)(p + 0xa) << 4);
}

#include "types.h"

extern u8 data_02047610[];

u32 func_0201c50c(u32 a0)
{
    return *(u16 *)((u8 *)((data_02047610 + (a0 * 0x1c))));
}

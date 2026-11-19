#include "types.h"

extern u8 *data_02047f00[];
u32 func_0201d2b4(u32 a0)
{
    u8 *g = *data_02047f00;
    u8 *t = *(u8 **)(g + 0x7c);

    if (a0 >= *(u32 *)(t + 8))
        return 0;
    return *(u32 *)(t + 0xc + a0 * 0x10);
}

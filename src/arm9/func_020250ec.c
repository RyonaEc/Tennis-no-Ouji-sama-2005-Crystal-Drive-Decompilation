#include "types.h"

extern u8 data_02048d3c[];
u8 *func_020250ec(void)
{
    u8 *p = *(u8 **)(data_02048d3c + 0xc);
    while (p != 0 && *(u32 *)(p + 0x64) != 1) p = *(u8 **)(p + 0x68);
    return p;
}

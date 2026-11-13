#include "types.h"

extern u8 data_02045664[];

u32 func_0201956c(void)
{
    u32 v0;
    v0 = *(u16 *)(data_02045664);
    *(u16 *)(data_02045664) = (v0 + 0x1);
    return v0;
}

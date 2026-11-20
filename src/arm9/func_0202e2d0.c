#include "types.h"

extern u8 data_0204b6e8[];

u32 func_0202e2d0(void)
{
    return *(u16 *)data_0204b6e8 ? 0 : 3;
}
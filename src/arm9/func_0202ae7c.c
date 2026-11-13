#include "types.h"

extern u8 data_0204ac68[];

u32 func_0202ae7c(u32 a0)
{
    return (*(u16 *)((u8 *)data_0204ac68 + 52) & a0);
}

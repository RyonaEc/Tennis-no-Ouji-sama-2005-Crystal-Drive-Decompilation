#include "types.h"

extern u8 data_0204ad00[];

u32 func_0202c448(u32 a0)
{
    *(u32 *)((u8 *)data_0204ad00 + 32) = a0;
    return a0;
}

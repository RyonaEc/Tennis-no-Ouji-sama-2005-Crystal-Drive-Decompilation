#include "types.h"

extern u8 data_02042368[];

u32 func_0201a548(u32 a0)
{
    *(u32 *)((u8 *)data_02042368) = a0;
    return a0;
}

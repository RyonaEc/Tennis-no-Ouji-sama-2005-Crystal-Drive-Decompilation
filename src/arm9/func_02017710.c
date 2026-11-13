#include "types.h"

extern u8 data_02042b04[];

u32 func_02017710(u32 a0)
{
    *(u32 *)((u8 *)data_02042b04) = a0;
    return a0;
}

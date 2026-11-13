#include "types.h"

extern u8 data_020475bc[];

u32 func_0201a56c(u32 a0)
{
    *(u32 *)((u8 *)data_020475bc) = a0;
    return a0;
}

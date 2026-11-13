#include "types.h"

extern u32 func_02015e64(u32);
extern u8 data_02042b20[];

u32 func_02015f40(u32 a0)
{
    *(u32 *)((u8 *)data_02042b20) = a0;
    return func_02015e64(0x3);
}

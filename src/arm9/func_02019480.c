#include "types.h"

extern u32 func_020195dc(u32, u32);

u32 func_02019480(u32 a0)
{
    return func_020195dc(*(u32 *)((u8 *)a0), *(u16 *)((u8 *)a0 + 4));
}

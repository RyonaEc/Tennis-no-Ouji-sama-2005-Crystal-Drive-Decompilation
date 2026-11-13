#include "types.h"

extern u32 func_02024ca8(u32, u32, u32, u32);
extern u8 func_02024c40[];

u32 func_02024c58(u32 a0)
{
    return func_02024ca8(a0, 0x27fffe8, (u32)func_02024c40, 0x1);
}

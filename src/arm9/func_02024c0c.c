#include "types.h"

extern u32 func_02024dc8(u32, u32, u32);
extern u8 func_02024bbc[];

u32 func_02024c0c(u32 a0)
{
    return func_02024dc8(a0, 0x27fffe0, (u32)func_02024bbc);
}

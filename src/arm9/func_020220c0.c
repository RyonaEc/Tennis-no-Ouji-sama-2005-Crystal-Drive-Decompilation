#include "types.h"

extern u32 func_02022224(u32, u32);
extern u32 func_020221b8(u32, u32);

u32 func_020220c0(u32 a0, u32 a1)
{
    u32 t0 = func_02022224(a0, a1);
    return func_020221b8(a0, 0x9);
}

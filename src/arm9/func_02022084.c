#include "types.h"

extern u32 func_020222a8(u32, u32, u32, u32, u32);
extern u32 func_020221b8(u32, u32);

u32 func_02022084(u32 a0, u32 a1, u32 a2, u32 a3)
{
    u32 t0 = func_020222a8(a0, 0x1b, a1, a2, a3);
    return func_020221b8(a0, 0x3);
}

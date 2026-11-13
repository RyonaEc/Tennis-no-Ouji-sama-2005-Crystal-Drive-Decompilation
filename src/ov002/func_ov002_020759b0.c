#include "types.h"

extern u32 func_ov002_020759d4(u32, u32, u32);

u32 func_ov002_020759b0(u32 a0, u32 a1, u32 a2)
{
    u32 t0;
    t0 = func_ov002_020759d4(a0, a1, a2);
    if (t0 != 0x0) {
        *(u8 *)((u8 *)(t0) + 16) = 0x1;
    }
    return t0;
}

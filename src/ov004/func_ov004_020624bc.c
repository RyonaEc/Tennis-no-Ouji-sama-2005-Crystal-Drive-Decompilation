#include "types.h"

extern u32 func_0200faa8(void);

u32 func_ov004_020624bc(u32 a0)
{
    u32 t0;
    t0 = func_0200faa8();
    if (t0 != 0x0) {
        *(u8 *)((u8 *)(t0) + 15) = (a0 + (-777));
    }
    return t0;
}

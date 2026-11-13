#include "types.h"

extern u32 func_0201f99c(u32, u32, u32);
extern u32 func_0201f980(u32);

u32 func_0201f950(u32 a0, u32 a1, u32 a2)
{
    u32 t0, t1;
    t0 = func_0201f99c(a0, a1, a2);
    if (t0 == 0x0) return 0x0;
    t1 = func_0201f980(a0);
    return 0x1;
}

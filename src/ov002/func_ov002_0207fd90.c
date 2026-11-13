#include "types.h"

extern u32 func_ov002_0207fdd0(u32);
extern u32 func_ov002_0207fcf0(u32, u32, u32);

u32 func_ov002_0207fd90(u32 a0, u32 a1, u32 a2)
{
    u32 t0, t1;
    t0 = func_ov002_0207fdd0(a0);
    if (a2 == 0x0) return 0x0;
    t1 = func_ov002_0207fcf0(t0, a1, (a2 - 0x1));
    return t1;
}

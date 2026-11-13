#include "types.h"

extern u32 func_02026ffc(u32, u32);
extern u32 func_02027bc0(u32);

u32 func_0201e124(u32 a0, u32 a1)
{
    u32 t0 = func_02026ffc(a0, (a0 + a1));
    return func_02027bc0(a0);
}

#include "types.h"

extern u32 func_02027028(u32, u32);
extern u32 func_02027c04(u32);

u32 func_0201e144(u32 a0, u32 a1)
{
    u32 t0 = func_02027028(a0, (a0 + a1));
    return func_02027c04(a0);
}

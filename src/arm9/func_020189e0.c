#include "types.h"

extern u32 func_020236f4(void);
extern u32 func_02023674(u32, u32, u32);
extern u32 func_02023610(u32);

u32 func_020189e0(u32 a0, u32 a1, u32 a2)
{
    u32 t0 = func_020236f4();
    u32 t1 = func_02023674(a0, a1, a2);
    return func_02023610(t1);
}

#include "types.h"

extern u32 func_020243e0(void);
extern u32 func_0202436c(u32, u32, u32);
extern u32 func_02024314(u32);

u32 func_02018b30(u32 a0, u32 a1, u32 a2)
{
    u32 t0 = func_020243e0();
    u32 t1 = func_0202436c(a0, a1, a2);
    return func_02024314(t1);
}

#include "types.h"

extern u32 func_0202745c(void);
extern u32 func_02027430(u32);

u32 func_02027410(void)
{
    u32 t0, t1;
    t0 = func_0202745c();
    t1 = func_02027430(t0);
    return ((0x100 - t0) - t1);
}

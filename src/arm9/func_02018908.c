#include "types.h"

extern u32 func_020233cc(void);
extern u32 func_02023358(u32, u32, u32);
extern u32 func_0202330c(u32);

u32 func_02018908(u32 a0, u32 a1, u32 a2)
{
    u32 t0 = func_020233cc();
    u32 t1 = func_02023358(a0, a1, a2);
    return func_0202330c(t1);
}

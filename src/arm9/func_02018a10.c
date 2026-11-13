#include "types.h"

extern u32 func_020235a0(void);
extern u32 func_0202352c(u32, u32, u32);
extern u32 func_020234d4(u32);

u32 func_02018a10(u32 a0, u32 a1, u32 a2)
{
    u32 t0 = func_020235a0();
    u32 t1 = func_0202352c(a0, a1, a2);
    return func_020234d4(t1);
}

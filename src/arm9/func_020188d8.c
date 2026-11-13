#include "types.h"

extern u32 func_020234b0(void);
extern u32 func_0202343c(u32, u32, u32);
extern u32 func_020233f0(u32);

u32 func_020188d8(u32 a0, u32 a1, u32 a2)
{
    u32 t0 = func_020234b0();
    u32 t1 = func_0202343c(a0, a1, a2);
    return func_020233f0(t1);
}

#include "types.h"

extern u32 func_0202d144(void);
extern u32 func_0202dc14(u32, u32, u32);
extern u32 func_0202d4f0(u32);

u32 func_0202d4bc(u32 a0)
{
    u32 t0, t1, t2;
    t0 = func_0202d144();
    *(u32 *)((u8 *)(*(u32 *)((u8 *)(a0))) + 4) = t0;
    t1 = func_0202dc14(a0, 0x2, 0x1);
    t2 = func_0202d4f0(a0);
    return t2;
}

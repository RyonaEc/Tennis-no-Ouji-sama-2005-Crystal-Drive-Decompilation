#include "types.h"

extern u32 func_020195f8(void);
extern u32 func_02019504(u32, u32, u32);

u32 func_02019380(void)
{
    u32 t0 = func_020195f8();
    return func_02019504(t0, 0x0, 0x20);
}

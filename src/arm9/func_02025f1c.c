#include "types.h"

extern u32 func_02025e48(u32);
extern u32 func_02025dc0(u32, u32);
extern u32 func_02025dd4(u32);
extern u32 func_02025dac(u32, u32);
extern u32 func_020260b0(u32);

u32 func_02025f1c(void)
{
    u32 t0 = func_02025e48(0x2);
    u32 t1 = func_02025dc0(0x2, t0);
    u32 t2 = func_02025dd4(0x2);
    u32 t3 = func_02025dac(0x2, t2);
    return func_020260b0(0x200002b);
}

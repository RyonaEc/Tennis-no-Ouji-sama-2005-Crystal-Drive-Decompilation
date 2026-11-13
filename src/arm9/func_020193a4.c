#include "types.h"

extern u32 func_02019614(void);
extern u32 func_02019504(u32, u32, u32);

u32 func_020193a4(void)
{
    u32 t0 = func_02019614();
    return func_02019504(t0, 0x0, 0x80);
}

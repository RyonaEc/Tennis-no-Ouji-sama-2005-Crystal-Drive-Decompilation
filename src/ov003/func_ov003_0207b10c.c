#include "types.h"

extern u32 func_ov003_02079eac(void);

u32 func_ov003_0207b10c(void)
{
    u32 t0;
    t0 = func_ov003_02079eac();
    *(u16 *)((u8 *)(0x400100c)) = ((*(u16 *)((u8 *)(0x400100c)) & ~0x3) | 0x3);
    return 0x0;
}

#include "types.h"

extern u32 func_02025c5c(u32, u32);
extern u8 data_0204ab60[];

u32 func_02027b18(void)
{
    u32 t0 = func_02025c5c(*(u32 *)((u8 *)data_0204ab60), 0x280);
    return *(u32 *)(*(u32 *)((u8 *)data_0204ab60) + 4);
}

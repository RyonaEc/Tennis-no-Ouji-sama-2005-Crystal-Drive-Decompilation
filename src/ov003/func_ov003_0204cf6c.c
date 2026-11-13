#include "types.h"

extern u32 func_02026d54(u32, u32);
extern u8 data_ov003_020eb724[];

u32 func_ov003_0204cf6c(void)
{
    u32 t0 = func_02026d54((*(u32 *)((u8 *)data_ov003_020eb724) + 0xc8000), (*(u32 *)((u8 *)data_ov003_020eb724) + 0xf0000));
    return 0x0;
}

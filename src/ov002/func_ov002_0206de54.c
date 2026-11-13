#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_0206de54(void)
{
    u32 v0;
    v0 = *(u32 *)(data_ov002_02102070);
    *(u8 *)((u8 *)((v0 + 0x1000)) + 4036) = 0x0;
    return (v0 + 0x1000);
}

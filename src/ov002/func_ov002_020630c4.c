#include "types.h"

extern u8 data_02042a68[];
extern u8 data_ov002_02102070[];

u32 func_ov002_020630c4(void)
{
    u32 v0, v1, v2;
    v0 = *(u32 *)(data_ov002_02102070);
    v1 = *(u16 *)(data_02042a68 + 52);
    v2 = *(u16 *)((u8 *)(v0));
    *(u16 *)((u8 *)(v0) + 2) = (v1 & (v1 ^ v2));
    *(u16 *)((u8 *)(v0)) = v1;
    return (v1 & (v1 ^ v2));
}

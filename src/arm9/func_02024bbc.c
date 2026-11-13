#include "types.h"



u32 func_02024bbc(void)
{
    u32 v0;
    v0 = *(u16 *)((u8 *)(0x4000204));
    *(u16 *)((u8 *)(0x4000204)) = (v0 & ~0x800);
    return (v0 & ~0x800);
}

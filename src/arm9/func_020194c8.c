#include "types.h"

extern s32 func_02019668(u8 *);

u32 func_020194c8(u8 *p)
{
    if (!func_02019668(p))
        return 0;
    return (u16)(*(u16 *)(p + 2) - *(u16 *)(p + 4) + 1);
}
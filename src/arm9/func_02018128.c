#include "types.h"

void func_02018128(u32 *out, u8 *p)
{
    out[0] = (u32)p - (u16)((*(u16 *)(p + 2) >> 8) & 0x7f);
    out[1] = *(u32 *)(p + 4) + ((u32)p + 0x10);
}

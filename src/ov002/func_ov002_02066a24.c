#include "types.h"

typedef struct { u32 a, b, c; } V;
extern u8 data_ov002_02102070[];
u32 func_ov002_02066a24(u32 a0, V *out)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x2e4 + a0 * 0x158);
    if (*(u32 *)(p + 0x4c) & 0x100000)
    {
        *out = *(V *)(p + 0xec);
        return 1;
    }
    return 0;
}

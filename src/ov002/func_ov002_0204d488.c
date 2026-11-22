#include "types.h"

extern u8 data_ov002_02102070;

typedef struct { u8 pad[0x30048]; u16 f; } Big;

u32 func_ov002_0204d488(void)
{
    u8 *b = *(u8 **)&data_ov002_02102070 + 0x1a89a4;
    Big *g = (Big *)b;

    if (g->f >= 0x1f) {
        g->f = 0;
    } else {
        u16 *d = (u16 *)(b + 0x30048);
        *d = *d + 1;
    }
    return g->f;
}
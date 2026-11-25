#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 data_ov002_020fdc24[];

typedef struct {
    u32 w[3];
} T3;

u32 func_ov002_02066080(u32 a0)
{
    T3 t;
    u8 *base;
    u8 *p;

    base = *(u8 **)data_ov002_02102070 + 0x1744;
    p = base + a0 * 0x38;
    t = *(T3 *)data_ov002_020fdc24;

    if (*(u32 *)(p + 0x18) < t.w[*p]) {
        *(u32 *)(p + 0x18) = *(u32 *)(p + 0x18) + 1;
        return 0;
    }
    return 1;
}
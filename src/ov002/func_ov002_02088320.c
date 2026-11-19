#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_02088320(void)
{
    u8 *g = *(u8 **)data_ov002_02102070;
    s32 i = 0;

    do {
        ((u32 *)(g + 0x1c))[i] = ~0u;
        i++;
    } while (i < 2);
}
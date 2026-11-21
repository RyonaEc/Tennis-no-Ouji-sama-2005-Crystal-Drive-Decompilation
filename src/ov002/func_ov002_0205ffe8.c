#include "types.h"

typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *data_ov002_02102070;

void func_ov002_0205ffe8(void)
{
    u8 *g = data_ov002_02102070;

    if (*(u32 *)(g + 0x28) & 1)
        return;
    if (*(u32 *)(g + 0x260) < 0x1464b40)
        (*(u32 *)(g + 0x260))++;
}
#include "types.h"

typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *data_ov002_02102070;

void func_ov002_0204f898(void)
{
    u8 *p = data_ov002_02102070 + 0x1824;
    u8 v = *(u8 *)(data_ov002_02102070 + 0x4f21);

    p[0x10c] = v;
    if (p[0x10c] == 0)
        (*(u32 *)(p + 0xcc))++;
}
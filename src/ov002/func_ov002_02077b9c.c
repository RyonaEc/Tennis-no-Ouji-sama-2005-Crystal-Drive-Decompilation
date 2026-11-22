#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_02077b9c(void)
{
    u8 *p = *(u8 **)data_ov002_02102070 + 0x1e6c;
    u16 i;

    for (i = 0; i < 10; i++)
        *(u16 *)(p + i * 0x50 + 0x264) = 0;
    for (i = 0; i < 7; i++)
        *(u8 *)(p + i + 0x584) = 0;
    for (i = 0; i < 4; i++)
        *(u32 *)(p + i * 8 + 0x58c) = 0;
}
#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_02059070(void)
{
    u8 *b = B;
    u8 *p = b + 0x2e4;
    s32 i;
    for (i = 0; i < 4; i++)
    {
        if (*(u8 *)p != 0)
        {
            u32 k = *(u8 *)(p + 2);
            if (*(u8 *)(b + (k >> 1) + 0x140) == 1)
                *(u8 *)(p + 0x12e) = 0x80;
        }
        p += 0x158;
    }
}

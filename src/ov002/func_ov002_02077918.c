#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_02077918(void)
{
    u8 *r = (u8 *)(*(u8 **)data_ov002_02102070 + 0x1e6c);
    u8 *q = r + 0x264;
    s32 i;
    for (i = 0; i < 0xa; i++)
    {
        if (*(u16 *)(r + 0x200 + 0x64) & 1)
            (*(void (**)(u8 *))(r + 0x270))(q);
        r += 0x50;
        q += 0x50;
    }
}

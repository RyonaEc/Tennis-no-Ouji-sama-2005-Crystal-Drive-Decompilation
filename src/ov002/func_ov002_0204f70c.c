#include "types.h"

extern s16 func_02020044(s32 y, s32 x);

u32 func_ov002_0204f70c(s32 a0, s32 a1)
{
    s32 v = func_02020044(a1, a0);

    if (v < 0)
        v = (s16)(v + 0x6488);
    return (u16)((v << 16) / 0x6488);
}
#include "types.h"

void func_ov009_0204db24(u16 *p, s32 a1, s32 a2)
{
    u16 *q;
    u16 v0, v1;

    q = p + 1;
    if (p == 0)
        return;
    v0 = p[0];
    v1 = *q;
    p[0] = (u16)(v0 & 0xff00) + (a2 & 0xff);
    *q = (u16)(v1 & 0xfe00) + (a1 & 0x1ff);
}
#include "types.h"

void func_02021aa8(u16 *p, s32 v)
{
    u16 f = *p;

    if (v < 0) {
        if ((f & 0xc0) == 0x80)
            *p = (f & ~0xc0) | 0xc0;
        p[2] = -v;
    } else {
        if ((f & 0xc0) == 0xc0)
            *p = (f & ~0xc0) | 0x80;
        p[2] = v;
    }
}
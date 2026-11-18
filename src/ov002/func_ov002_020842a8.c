#include "types.h"

void func_ov002_020842a8(s32 idx, s32 count)
{
    u8 *p;
    s32 i;

    p = (u8 *)func_ov002_02084888(idx) + idx * 8;
    i = 0;
    while (i < count) {
        *(u32 *)p = 0x200;
        i++;
        p += 8;
    }
}

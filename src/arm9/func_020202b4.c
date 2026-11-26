#include "types.h"

extern u16 data_02048c70;
extern u16 data_0204236c;

void func_020202b4(void)
{
    u16 v;
    u32 r;

    data_0204236c = 1;
    v = data_02048c70;
    if (v != 0) {
        r = (*(u32 *)0x04000000 & ~0x30000) | (v << 16);
        *(u32 *)0x04000000 = r;
        return;
    }
    *(u32 *)0x04000000 = *(u32 *)0x04000000 | 0x10000;
}
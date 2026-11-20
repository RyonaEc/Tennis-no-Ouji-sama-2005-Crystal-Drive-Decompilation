#include "types.h"

extern u32 data_ov009_0205333c[];

void func_ov009_0204e738(s32 a0, s32 a1)
{
    if (a1 == 0) {
        *(u32 *)0x04000488 = data_ov009_0205333c[a0];
        return;
    }
    if (a1 == 1) {
        *(u32 *)0x04000488 = data_ov009_0205333c[a0 + 4];
        return;
    }
    if (a1 == 2) {
        *(u32 *)0x04000488 = data_ov009_0205333c[a0 + 8];
        return;
    }
    if (a1 == 3) {
        *(u32 *)0x04000488 = data_ov009_0205333c[a0 + 12];
        return;
    }
    if (a1 == 4)
        *(u32 *)0x04000488 = data_ov009_0205333c[a0 + 16];
}
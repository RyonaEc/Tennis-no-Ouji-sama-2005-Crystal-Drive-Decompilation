#include "types.h"

extern u32 data_ov005_02079900[];

void func_ov005_02056908(u32 a0, u32 a1)
{
    if (a1 == 0) {
        *(u32 *)0x04000488 = data_ov005_02079900[a0];
        return;
    }
    if (a1 == 1) {
        *(u32 *)0x04000488 = data_ov005_02079900[a0 + 4];
        return;
    }
    if (a1 == 2) {
        *(u32 *)0x04000488 = data_ov005_02079900[a0 + 8];
        return;
    }
    if (a1 == 3) {
        *(u32 *)0x04000488 = data_ov005_02079900[a0 + 0xc];
        return;
    }
    if (a1 == 4)
        *(u32 *)0x04000488 = data_ov005_02079900[a0 + 0x10];
}
#include "types.h"

extern u16 data_ov005_02079950[];

void func_ov005_020569c0(u32 a0, u32 a1)
{
    u32 i;

    if (a1 == 0) {
        i = a0 * 3;
        *(u32 *)0x0400048c = data_ov005_02079950[i] |
                             (data_ov005_02079950[i + 1] << 16);
        *(u32 *)0x0400048c = 0;
        return;
    }
    if (a1 == 1) {
        i = a0 * 3;
        *(u32 *)0x0400048c = data_ov005_02079950[i + 0x18] |
                             (data_ov005_02079950[i + 0x19] << 16);
        *(u32 *)0x0400048c = 0;
        return;
    }
    if (a1 == 2) {
        i = a0 * 3;
        *(u32 *)0x0400048c = data_ov005_02079950[i + 0x30] |
                             (data_ov005_02079950[i + 0x31] << 16);
        *(u32 *)0x0400048c = 0;
        return;
    }
}
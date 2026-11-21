#include "types.h"

extern s16 data_0203d908[];

void func_ov001_0204ebb0(s32 a0)
{
    if (a0 > 0x40) {
        s32 idx = ((0x28 - (a0 - 0x40)) * 0x199) >> 4;
        s16 v = data_0203d908[idx * 2];
        s32 t = (v * 0x120) >> 12;

        *(volatile u32 *)0x04000018 = (u32)((t - 0xc0) << 16) & 0x1ff0000;
        *(volatile u32 *)0x04001018 = (u32)(t << 16) & 0x1ff0000;
    } else {
        s32 h = a0 >> 1;

        *(volatile u32 *)0x04000018 = (u32)((0x80 - h) << 16) & 0x1ff0000;
        *(volatile u32 *)0x04001018 = (u32)((0x140 - h) << 16) & 0x1ff0000;
    }
}
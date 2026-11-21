#include "types.h"

extern s16 data_0203d908[];

void func_ov001_0204efa0(s32 a0)
{
    s32 idx = ((0x20 - a0) << 9) >> 4;
    s32 v = ((s32)data_0203d908[idx * 2] << 8) >> 12;

    *(volatile u32 *)0x04000014 = (v + 10) & 0x1ff;
    *(volatile u32 *)0x04000010 = (0x100 - v) & 0x1ff;
}
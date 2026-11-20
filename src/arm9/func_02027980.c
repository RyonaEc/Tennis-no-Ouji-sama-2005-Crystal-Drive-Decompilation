#include "types.h"

extern u8 data_0204ab00[];

void func_02027980(s32 a0) {
    u8 *e;

    e = data_0204ab00 + (a0 & 0xff) * 0xc;
    if (((a0 >> 8) & 0xff) != e[8])
        return;
    if (*(u32 *)e == 0)
        return;
    ((void (*)(u32)) * (u32 *)e)(*(u32 *)(e + 4));
}
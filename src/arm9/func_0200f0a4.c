#include "types.h"

extern u8 data_02042a68[];

void func_0200f0a4(s32 a0, s32 *a1, s32 *a2)
{
    s32 i;

    *a2 = 0;
    *a1 = *a2;
    for (i = 0; i < a0; i++) {
        u8 *p = *(u8 **)(data_02042a68 + 0x10) + i;

        if ((*(u8 *)(p + 0x12c) & 0x80) != 0) {
            (*a1)++;
            continue;
        }
        if ((*(u8 *)(p + 0x134) & 0x80) != 0) {
            (*a2)++;
            continue;
        }
        if (*(u8 *)(p + 0x12c) > *(u8 *)(p + 0x134)) (*a2)++;
        else (*a1)++;
    }
}
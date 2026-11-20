#include "types.h"

extern s16 data_0203d908[];

void func_ov003_0205220c(u8 *a0, s32 a1)
{
    s16 *t;
    s32 idx;
    s32 v0;
    s16 v1;

    idx = a1 >> 4;
    t = data_0203d908;
    v1 = t[idx * 2 + 1];
    v0 = t[idx * 2];
    *(s16 *)(a0 + 6) = v1 >> 4;
    *(s16 *)(a0 + 0xe) = v0 >> 4;
    *(s16 *)(a0 + 0x16) = -v0 >> 4;
    *(s16 *)(a0 + 0x1e) = v1 >> 4;
}

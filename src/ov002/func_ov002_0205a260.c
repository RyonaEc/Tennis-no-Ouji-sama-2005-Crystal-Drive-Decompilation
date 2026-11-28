#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 data_ov002_020fb544[];
extern s32 func_02034148(s32 a0);

typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
} Src;

typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
} Dst;

void func_ov002_0205a260(u32 id, s16 start, s16 end)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    s32 *tbl = (s32 *)(data_ov002_020fb544 + *(u8 *)(base + id * 0x158 + 0x2e4) * 0x34);
    Dst *dst = (Dst *)(base + 0x844 + id * 0x3c0);
    Src *src = (Src *)(base + 0x1934);
    s16 i;

    if (start > end)
        return;
    src += start;
    i = start;
    do {
        dst[i].w7 = dst[i].w2 - src->w3 + tbl[2] / 2;
        dst[i].w4 = func_02034148(dst[i].w0 - src->w1);
        dst[i].w5 = func_02034148(dst[i].w1 - src->w2);
        dst[i].w6 = func_02034148(dst[i].w7);
        src++;
        i++;
    } while (i <= end);
}
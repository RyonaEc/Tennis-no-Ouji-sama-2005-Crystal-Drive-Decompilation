#include "types.h"

extern u8 data_ov002_02102070[];
extern s32 func_ov002_02062dbc(u32 a0);

void func_ov002_0206a030(void)
{
    u8 *g = *(u8 **)data_ov002_02102070 + 0x1ddbfc;
    u8 *e = g + 0x14 + *(u32 *)(g + 4) * 0x20;
    s32 v;

    *(u32 *)e = 1;
    *(u32 *)(e + 4) = 0;
    *(u32 *)(e + 0x10) = -0x32000;
    v = func_ov002_02062dbc(0x100);
    *(u32 *)(e + 8) = ((v << 16) / 256) & 0xffff;
    v = func_ov002_02062dbc(0x100);
    *(s16 *)(e + 0x14) = (v << 16) / 256;
    v = func_ov002_02062dbc(0x100);
    *(s16 *)(e + 0x16) = (v << 16) / 256;
    v = func_ov002_02062dbc(0x100);
    *(s16 *)(e + 0x18) = (v << 16) / 256;
    if (*(u32 *)(g + 4) < 0x3f)
        *(u32 *)(g + 4) += 1;
    else
        *(u32 *)(g + 4) = 0;
}
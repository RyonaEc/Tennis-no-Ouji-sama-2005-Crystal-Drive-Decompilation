#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_0206e254(u32);

void func_ov002_0206e4a8(void)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *q = p + 0x2e4;
    u8 *t = p + 0x1e6c;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (*(u8 *)q != 0 && *(u8 *)(q + 0xd1) == 0 && *(u8 *)(t + i + 0x8c) == 1)
            func_ov002_0206e254(i & 0xff);
        q += 0x158;
    }
}

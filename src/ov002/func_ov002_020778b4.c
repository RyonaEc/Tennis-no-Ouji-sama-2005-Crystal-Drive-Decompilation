#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_020778b4(void)
{
    s32 i;
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *a = p + 0x1e6c;
    u8 *b = a + 0x264;

    for (i = 0; i < 10; i++) {
        u16 v = *(u16 *)(a + 0x264);

        if ((v & 1) && (v & 0x80))
            (*(void (**)(u8 *))(a + 0x270))(b);
        a += 0x50;
        b += 0x50;
    }
}

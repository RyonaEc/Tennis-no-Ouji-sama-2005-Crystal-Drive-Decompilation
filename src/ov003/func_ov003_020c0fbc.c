#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_ov003_02052100(u32 a0, u16 a1, u16 a2, u16 *p);
extern void func_ov003_020c1ab0(u32 a0);

u32 func_ov003_020c0fbc(void)
{
    u8 *b = *(u8 **)data_ov003_020eb720;
    u16 i;
    u16 j;

    if (*(u16 *)(b + 0x8736) < 0x20) {
        u16 *q = (u16 *)(b + 0xadf2);

        for (i = 0; i < 2; i++)
            *q = *q - 1;
        for (j = 0x64; j < 0x70; j++) {
            u8 *p = b + 0x91e8 + j * 0x1c;

            if (*(s32 *)(p + 4) < 0xc0)
                *(s32 *)(p + 4) = *(s32 *)(p + 4) + 2;
            func_ov003_02052100(*(u32 *)(p + 0x10), *(u32 *)p, *(u32 *)(p + 4), (u16 *)p);
        }
        func_ov003_020c1ab0(0);
    } else {
        *(u16 *)(b + 0x86c2) = 0;
        *(u16 *)(b + 0x86c0) = 0x4a;
        *(u16 *)(b + 0x8718) = 0x1f4;
    }
    *(u16 *)(b + 0x8736) += 1;
    return 1;
}
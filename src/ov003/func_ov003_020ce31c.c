#include "types.h"

extern u8 *data_ov003_020eb720;
extern void func_ov003_02052100(u16 *p, u32 a1, u32 a2);

void func_ov003_020ce31c(void)
{
    u8 *b = data_ov003_020eb720;
    u16 i;

    for (i = 0x78; i < 0x7c; i++) {
        u32 *p = (u32 *)(b + 0x9fe8 + i * 0x1c);

        func_ov003_02052100((u16 *)p[4], (u16)p[0], (u16)p[1]);
    }
}
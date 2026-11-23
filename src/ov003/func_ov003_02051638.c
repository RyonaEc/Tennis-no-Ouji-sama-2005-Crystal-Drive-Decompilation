#include "types.h"

extern u32 func_02008c60(void);
extern void func_ov003_02052100(u32, u32, u32);
extern u32 data_ov003_020eb720;

void func_ov003_02051638(void)
{
    u8 *base = (u8 *)data_ov003_020eb720;
    u16 n = (u16)func_02008c60();
    u16 i;

    if ((u16)(n + 0xffff) > 1)
        return;
    for (i = 0x7c; i < 0x7e; i++)
        func_ov003_02052100(*(u32 *)(base + i * 0x1c + 0x9ff8), 0, 0xc0);
}

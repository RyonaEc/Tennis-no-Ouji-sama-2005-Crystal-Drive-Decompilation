#include "types.h"

extern u8 data_ov003_020eb720[];
extern u32 func_0200fd40(void);
extern void func_ov003_02052100(u16 *p, u32 a1, u32 a2);

s32 func_ov003_020af6d4(void)
{
    u8 *base = *(u8 **)data_ov003_020eb720;
    u16 t[2] = { 0, 0 };
    u16 i;
    s32 rv = func_0200fd40();
    u8 *q;
    u8 *e;

    *(u16 *)(base + 0x8986) = 0;
    *(u16 *)(base + 0x897e) = 0x10;
    *(u16 *)(base + 0xba0c) = 1;
    for (i = 0; i < 0x1b; i++) {
        e = base + 0x9fe8 + i * 0x1c;
        func_ov003_02052100(*(u16 **)(e + 0x10), (u16)*(u32 *)e,
                            (u16)*(u32 *)(e + 4));
    }
    q = base + 0xa2dc;
    *(u32 *)(*(u32 *)(q + 0x10)) |= 0x400;
    if (rv < 0)
        func_ov003_02052100(*(u16 **)(q + 0x10), (u16)*(u32 *)q,
                            (u16)*(u32 *)(q + 4));
    else
        func_ov003_02052100(*(u16 **)(q + 0x10), 0, 0xc0);
    (void)t;
    return 0;
}
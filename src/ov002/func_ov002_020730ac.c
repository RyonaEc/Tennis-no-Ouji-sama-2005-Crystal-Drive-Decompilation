#include "types.h"

extern void func_ov002_02072db0(void);
extern u8 *func_ov002_02077974(void *a0, u32 a1, u32 a2, u32 a3, u32 a4);
extern u32 func_ov002_0204f70c(s32 a0, s32 a1);

u8 *func_ov002_020730ac(u32 a0, u8 *a1)
{
    u8 *p = func_ov002_02077974((void *)func_ov002_02072db0, 0x14, 0, 0, 0);

    if (p != 0) {
        u32 *q = (u32 *)(p + 0x10);

        q[1] = a0;
        q[2] = 0;
        q[3] = (u16)(func_ov002_0204f70c((s16)(*(u32 *)(a1 + 0x60) >> 8), (s16)(*(u32 *)(a1 + 0x5c) >> 8)) >> 8);
        q[4] = *(u32 *)(a1 + 0xb8) & 2;
    }
    return p;
}
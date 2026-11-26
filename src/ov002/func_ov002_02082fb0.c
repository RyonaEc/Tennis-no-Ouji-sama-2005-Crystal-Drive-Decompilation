#include "types.h"

extern u32 data_ov002_02102070[];
extern void func_ov002_0208381c(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7);

void func_ov002_02082fb0(u32 a0, s32 a1, s32 a2)
{
    u8 *base = (u8 *)data_ov002_02102070[0] + 0x4000;
    u8 *t0 = *(u8 **)(base + 0xf6c);
    u8 *t1 = *(u8 **)(base + 0xf68);
    u8 *t2 = *(u8 **)(base + 0xf64);
    u8 *t3 = *(u8 **)(base + 0xf60);
    u32 i, k, j, n;

    if (a0 > 3) {
        if (a2 == 0)
            return;
    }
    i = (a2 == 1) ? 0xe : a0 + 0xa;
    k = *(u16 *)(t0 + i * 4);
    j = *(u16 *)(t1 + k * 4);
    n = *(u16 *)(t2 + j * 4);
    func_ov002_0208381c((u32)(t2 + j * 4), (u32)(t3 + n * 0x14),
                        (u16)(a1 * 0x12 + 0xd1), 0xa, 0, 0x6400000, 0, 0);
}
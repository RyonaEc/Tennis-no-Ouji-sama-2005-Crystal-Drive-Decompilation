#include "types.h"

extern u8 data_02047e18[];
extern s32 func_02027b4c(u32 a0);
extern void func_02027104(u32 a0, u32 a1, u32 a2);

void func_0201cae0(u8 *a0, u32 a1)
{
    u32 id;
    s32 i;
    s32 v;

    *(u32 *)(a0 + 0x3c) = a1;
    for (i = 0; i < *(s32 *)(a0 + 0x48); i++) {
        id = *(u8 *)(a0 + i + 0x4c);
        v = func_02027b4c(*(u32 *)(a0 + 0x3c)
                          + *(u32 *)(data_02047e18 + id * 8 + 4));
        func_02027104(1 << id, v & 0xff, (u32)(v >> 8));
    }
}
#include "types.h"

extern u8 data_0204b6ec[];
extern s32 func_0202e2d0(void);
extern void func_02025c5c(u32 a0, u32 a1);

s32 func_0202e274(void)
{
    s32 r = func_0202e2d0();

    if (r != 0)
        return r;
    func_02025c5c(*(u32 *)(*(u32 *)data_0204b6ec + 4), 2);
    if (*(u16 *)(*(u32 *)(*(u32 *)data_0204b6ec + 4)) <= 1)
        return 3;
    return 0;
}
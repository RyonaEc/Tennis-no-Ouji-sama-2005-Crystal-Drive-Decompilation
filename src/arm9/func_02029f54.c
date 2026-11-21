#include "types.h"

extern u32 data_0204ac00;
extern void func_02029fa0(void);
extern s32 func_0202d5ec(u32 a0, u32 a1, u32 a2, u32 a3, void *fn, u32 a5, u32 a6);

s32 func_02029f54(u32 a0, u32 a1, u32 a2, u32 a3)
{
    func_0202d5ec(data_0204ac00, a2, a1, a3, func_02029fa0, a0, 1);
    return 6;
}
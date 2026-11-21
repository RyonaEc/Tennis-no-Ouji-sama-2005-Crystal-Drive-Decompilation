#include "types.h"

extern u16 data_0204c180[];
extern s32 func_02024b04(void);
extern void CpuSet(void *src, void *dest, u32 mode);

void func_020307c8(void)
{
    u32 v = 0;

    CpuSet(&v, data_0204c180, 0x05000001);
    *(u16 *)((u8 *)data_0204c180 + 2) = func_02024b04();
}
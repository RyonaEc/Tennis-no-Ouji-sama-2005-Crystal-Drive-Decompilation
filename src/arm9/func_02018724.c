#include "types.h"

extern u8 data_02045650[];
extern u32 func_02018808(u32);
extern u32 func_020187b0(u32);
extern void func_0201887c(u32);

void func_02018724(void)
{
    u8 *d = data_02045650;
    u32 v = func_02018808((u32)d);

    while (func_020187b0((u32)d) != 0)
    {
        func_0201887c(v);
        *(u32 *)(d + 0x10) = *(u32 *)(d + 0x10) - *(u32 *)(v + 0xc);
        v = func_02018808((u32)d);
    }
}
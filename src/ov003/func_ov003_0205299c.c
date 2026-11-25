#include "types.h"

extern u8 data_027c0000;
extern u8 data_ov003_020eb720[];

void func_ov003_0205299c(void)
{
    void (**fn)(void) = *(void (***)(void))data_ov003_020eb720;

    if (*fn != 0)
        (*fn)();
    *(u32 *)((u8 *)&data_027c0000 + 0x3ff8) |= 1;
}
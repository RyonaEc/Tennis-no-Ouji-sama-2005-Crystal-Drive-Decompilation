#include "types.h"

extern u8 *func_ov002_020848a0(void);
void func_ov002_02086f7c(void)
{
    u8 *p = func_ov002_020848a0();
    s32 i;
    for (i = 0; i < 4; i++)
    {
        *(u32 *)p = 0x200;
        p += 8;
    }
}

#include "types.h"

extern u8 *func_ov008_0204ed28(void);

void func_ov008_0204e75c(s32 idx, s32 count)
{
    u8 *t = func_ov008_0204ed28();
    u8 *p = t + idx * 8;
    s32 i = 0;

    if (count <= 0)
        return;
    for (;;) {
        *(u32 *)p = 0x200;
        i++;
        p += 8;
        if (i >= count)
            return;
    }
}

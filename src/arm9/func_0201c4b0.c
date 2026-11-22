#include "types.h"

extern u8 data_020475fc[];

extern u8 *func_02017784(u8 *, u8 *);

s32 func_0201c4b0(u32 a0)
{
    u8 *p;
    s32 n = 0;

    p = func_02017784(data_020475fc, 0);
    while (p != 0) {
        if (*(u16 *)(p + 0x34) == 1 && *(u16 *)(p + 0x38) == a0)
            n++;
        p = func_02017784(data_020475fc, p);
    }
    return n;
}
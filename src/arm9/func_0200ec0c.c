#include "types.h"

extern u8 data_0203cb6c[];

s32 func_0200ec0c(u32 a0)
{
    s32 i;
    u8 *p = data_0203cb6c;

    for (i = 0; i < 0x40; i++, p += 0x14) {
        if (*(u16 *)(p + 4) <= 1) {
            if (a0 == *(u16 *)(p + 0xe)) return i;
        }
    }
    return 0x40;
}
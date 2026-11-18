#include "types.h"

extern u8 data_ov010_0205e500[];
#define G (*(u8 **)data_ov010_0205e500)

s32 func_ov010_020524cc(s32 n)
{
    u16 bits = *(u16 *)(G + 0x15c);
    s32 i;
    s32 count = 0;

    for (i = 1; i < 0x10; i++) {
        if (bits & (1 << i)) {
            count++;
            if (count == n)
                return i;
        }
    }
    return 0;
}
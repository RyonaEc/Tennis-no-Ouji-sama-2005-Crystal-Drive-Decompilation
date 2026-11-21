#include "types.h"

extern u8 *data_ov003_020eb720;

s32 func_ov003_0205e028(u32 a0, u32 a1)
{
    u8 *g = data_ov003_020eb720;
    u8 *p;
    if (a1 == 0) {
        if (a0 == 1) {
            return (*(u16 *)(g + 0x29c) - 1 == 0 || *(u16 *)(g + 0x29e) - 1 == 0) ? 1 : 2;
        }
        p = g + 0x200;
        if (*(u16 *)(p + 0x9c) - 1 == 0)
            return 1;
        return 2;
    }
    if (a0 == 1) {
        return (*(u16 *)(g + 0x29c) - 1 == 0 || *(u16 *)(g + 0x29e) - 1 == 0) ? 2 : 1;
    }
    p = g + 0x200;
    if (*(u16 *)(p + 0x9c) - 1 == 0)
        return 2;
    return 1;
}
#include "types.h"

extern u8 data_ov003_020eb720[];
extern u32 func_02008c60(void);

u32 func_ov003_02071714(void)
{
    u8 *base = *(u8 **)data_ov003_020eb720;
    u8 *t = base + 0xb8dc;

    if (func_02008c60() == 0) {
        if ((*(u16 *)(base + 0x352) == 4 && *(u16 *)(t + 0xa4) == 0
             && *(u16 *)(t + 0x14 + *(u16 *)(t + 0xa2) * 2) == 4
             && *(u16 *)(t + 0xa2) == 0)
            || (*(u16 *)(t + 0xa4) == 1 && *(u16 *)(t + 0xa2) == 0))
            return 1;
    } else if (*(u16 *)(base + 0x352) == 4
               && ((*(u16 *)(t + 0xa4) == 0
                    && *(u16 *)(t + 0x14 + *(u16 *)(t + 0xa2) * 2) == 4
                    && *(u16 *)(t + 0xa2) == 0)
                   || (*(u16 *)(t + 0xa4) == 1
                       && *(u16 *)(t + 0x14 + *(u16 *)(t + 0xa2) * 2) == 4
                       && *(u16 *)(t + 0xa2) == 0))) {
        return 1;
    }
    return 0;
}
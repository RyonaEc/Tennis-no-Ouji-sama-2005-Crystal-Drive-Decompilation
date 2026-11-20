#include "types.h"

extern u8 *data_ov003_020eb720;

s32 func_ov003_02053aa4(void)
{
    u32 v;

    v = *(u16 *)(data_ov003_020eb720 + 0x352);
    switch (v) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 2;
    case 4:
        return 3;
    }
    return 0;
}

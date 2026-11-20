#include "types.h"

extern u8 *data_ov003_020eb720;

s32 func_ov003_02067084(void) {
    u16 v;

    v = *(u16 *)(data_ov003_020eb720 + 0xbc00 + 0x24);
    if (v == 0 || v == 1 || v == 2 || v == 3 || v == 4 || v == 5 || v == 0x2a ||
        v == 0x1a || v == 0x1b || v == 0x1c || v == 0x1d || v == 0x1e ||
        v == 0x1f || v == 0x20 || v == 0x26)
        return 1;
    return 0;
}
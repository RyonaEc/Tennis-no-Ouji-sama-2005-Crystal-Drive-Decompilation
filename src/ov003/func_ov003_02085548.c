#include "types.h"

extern u8 *data_ov003_020eb720;
extern u8 data_02042a68[];

void func_ov003_02085548(void)
{
    u8 *g;
    u16 v;
    u16 f16;

    g = data_ov003_020eb720;
    v = *(u16 *)(g + 0xbaf8);
    if (v <= 3)
        return;
    if (v == 0x19)
        return;
    f16 = *(u16 *)(data_02042a68 + 0x30);
    if (f16 & 1) {
        if (v <= 0x19)
            return;
        if (*(u16 *)(g + 0x8706) == 0)
            *(u16 *)(g + 0x8706) = 1;
        return;
    }
    if (v < 0x19) {
        *(u16 *)(g + 0x8706) = 2;
        *(u16 *)(g + 0x8704) = 0;
        return;
    }
    if (v > 0x19) {
        *(u16 *)(g + 0x8706) = 2;
        *(u16 *)(g + 0x8704) = 0;
    }
}
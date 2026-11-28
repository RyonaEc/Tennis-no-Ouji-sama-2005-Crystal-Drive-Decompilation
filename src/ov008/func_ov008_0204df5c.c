#include "types.h"

extern u16 data_02042ab4[];
extern u8 *data_ov008_0204f440[];
extern s32 func_02034148(s32 a0);

void func_ov008_0204df5c(void)
{
    u32 bits = 0;

    u16 *p = data_02042ab4;

    if (p[3] == 0 && p[2] == 1) {
        s32 i;
        s32 x;
        s32 y;

        x = p[0];
        y = p[1];

        for (i = 0; i < 9; i++) {
            if (func_02034148(x - ((i % 3) * 0x50 + 0x30)) < 0x20) {
                if (func_02034148(y - ((i / 3) * 0x38 + 0x28)) < 0x18)
                    bits |= 1 << i;
            }
        }
        if (x < 0x80) {
            if (y > 0x91)
                bits |= 0x200;
        }
        {
            s32 dy = y - 0x92;
            s32 dx = x - 0xe0;
            if (dx * dx + dy * dy < 0x100)
                bits |= 0x400;
        }
    }
    *(u32 *)(data_ov008_0204f440[0] + 0x3c) = bits;
}
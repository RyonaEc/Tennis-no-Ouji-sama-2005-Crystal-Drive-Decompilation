#include "types.h"

extern void func_0200e8f4(u32 a0, u8 *a1);

void func_ov003_0204cd8c(void)
{
    u16 i;

    for (i = 0; i < 4; i++) {
        u8 buf[7];

        buf[0] = 0xff;
        buf[1] = 0xff;
        buf[2] = 0xff;
        buf[3] = 0xff;
        buf[4] = 0xff;
        buf[5] = 0x2f;
        buf[6] = 0;
        func_0200e8f4(i, buf);
    }
}
#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_0203447c(void *a0, s32 a1, u32 a2);

void func_ov002_02084244(void)
{
    u8 *s = *(u8 **)data_ov002_02102070 + 0x36ac;
    u8 *base = *(u8 **)data_ov002_02102070;

    func_0203447c(s, 0, 0x2c);
    s[0x25] = 0;
    s[0x27] = 0;
    s[0x2a] = 0;
    if (base[0xbc] == 2) {
        s[5] = base[0xbd];
        s[6] = base[0xbe];
    }
}
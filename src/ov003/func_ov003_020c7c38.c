#include "types.h"

extern u8 *data_ov003_020eb720;

void func_0200880c(u32 a0, u16 *a1);
void func_ov003_020c75d4(u32 a0, u32 a1);

s32 func_ov003_020c7c38(u16 *p) {
    u8 *base = data_ov003_020eb720;
    u8 *g = base + 0xb8dc;
    u16 n = *p;
    u16 buf[0x28];
    u16 i;

    if (n == 9) {
        func_0200880c(*(u16 *)(g + 0x100 + *(u16 *)(g + 0x100 + 0xa8) * 2 + 0x1e), buf);
        if (buf[0] != 0) {
            for (i = 0; i < buf[0]; i++)
                *(u16 *)(g + 0x100 + i * 2 + 0x36) = *(u16 *)((u8 *)buf + i * 2 + 2);
        }
        if (*(u16 *)(g + 0xa) == 0)
            func_ov003_020c75d4(1, 0);
        else
            func_ov003_020c75d4(0, 0);
    }
    if (n != 0)
        n = n - 1;
    *p = n;
    return 0;
}
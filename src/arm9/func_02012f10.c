#include "types.h"

extern u8 data_02042a68[];
extern u32 func_02014670(u32 a0);
extern u8 *func_02014d34(u32 a0);
extern s32 func_020145b8(s32 a0, s32 a1);
extern void func_02026b88(const void *src, void *dst, u32 size);
extern void func_0200e8f4(s32 a0, u8 *a1);

u32 func_02012f10(void)
{
    u8 *p = *(u8 **)(*(u32 *)(data_02042a68 + 0x14) + 0x104);
    u8 buf[8];
    s32 i;
    u8 b;

    *p = 6;
    if (func_02014670(6) == 0) {
        p[1] = 0;
        return 0;
    }
    if (func_02014d34(0)[1] == 3)
        return 2;
    if (func_020145b8(1, 0) == 0) {
        if (func_020145b8(1, 1) != 0)
            return 3;
        return 0;
    }
    for (i = 0; i < 4; i++) {
        b = *(u8 *)(*(u32 *)(data_02042a68 + 0x14) + i + 0x24);
        if (b == 0xff)
            continue;
        func_02026b88(func_02014d34(b) + 2, buf, 8);
        func_0200e8f4(i, buf);
    }
    return 1;
}
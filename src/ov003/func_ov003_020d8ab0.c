#include "types.h"

extern u8 *data_ov003_020eb720;

void func_ov003_0204f0c8(u32 a0);
u32 func_ov003_020da274(u32 a0);
void func_02006c60(void);
s32 func_02008178(s32 a0, s32 a1);
s32 func_0200fcac(s32 a0);

s32 func_ov003_020d8ab0(void) {
    u8 *base = data_ov003_020eb720;
    u16 *p;

    if (*(u16 *)(base + 0x8736) < 0x20) {
        if (*(u16 *)(base + 0x8736) < 2)
            func_ov003_0204f0c8(*(u16 *)(base + 0x32));
        if (*(u16 *)(base + 0x8808) < 0x20) {
            p = (u16 *)(base + 0x8808);
            *p = *p + 1;
        }
        *(u16 *)(base + 0x36) = 1;
        func_ov003_020da274(0);
    } else {
        *(u16 *)(base + 0x8718) = 0;
        if (*(u16 *)(base + 0x86ce) == 1) {
            *(u16 *)(base + 0x86c2) = 1;
            *(u16 *)(base + 0x86c0) = 0x38;
        } else {
            func_02006c60();
            func_0200fcac(func_02008178(0xb, *(s32 *)(base + 0x348)));
            *(u16 *)(base + 0x86c2) = 0;
            *(u16 *)(base + 0x86c0) = 0x44;
        }
    }
    p = (u16 *)(base + 0x8736);
    *p = *p + 1;
    return 1;
}
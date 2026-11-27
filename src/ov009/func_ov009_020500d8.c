#include "types.h"

extern u8 *data_ov009_02054f84;
extern u8 data_ov009_02052adc[];

u32 func_ov009_02051078(u32 a0);
void func_ov009_0204db24(u16 *p, s32 a1, s32 a2);
void func_02025c94(u32 a0, u32 a1);
u32 func_02023e58(u32 a0, u32 a1, u32 a2);
void func_02006584(s32 arg);

s32 func_ov009_020500d8(void) {
    u8 *base = data_ov009_02054f84;
    u16 i;
    u16 *p;

    if (*(u16 *)(base + 0x36e) < 0x10) {
        *(u16 *)(base + 0x36) = 2;
        func_ov009_02051078(0);
        for (i = 0; i < 2; i++)
            func_ov009_0204db24((u16 *)*(u32 *)(base + i * 0x1c + 0xe30), 0, 0xc0);
        func_02025c94((u32)data_ov009_02052adc, 0x200);
        func_02023e58((u32)(void *)data_ov009_02052adc, 0xbb80, 0x200);
    } else {
        *(u16 *)(base + 0x2fa) = 0;
        *(u16 *)(base + 0x2f8) = 0x65;
        func_02006584(0x20);
        *(u16 *)(base + 0x350) = 0;
    }
    p = (u16 *)(base + 0x36e);
    *p = *p + 1;
    return 1;
}
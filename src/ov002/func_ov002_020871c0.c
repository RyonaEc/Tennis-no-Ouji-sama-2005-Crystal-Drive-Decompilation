#include "types.h"

extern u8 *data_ov002_02102070;

void func_ov002_020870fc(void);
void func_ov002_02060688(u32 a0, u32 a1, u32 a2);
void func_ov002_02086f14(void);

void func_ov002_020871c0(void) {
    u8 *base = data_ov002_02102070;
    u8 *p = base + 0x47d0;
    u8 *q = base + 0x2e4;

    if (*(u8 *)(p + 2) == 1 && *(u8 *)(p + 5) == 0 && (*(u16 *)(base + 2) & 1)) {
        if (*(u8 *)(p + 1) == 0) {
            if (*(u8 *)(base + 0xc1) == 1)
                func_ov002_020870fc();
            else {
                *(u8 *)(base + 0x24) = 1;
                func_ov002_02060688(0, 0, 0);
            }
        } else {
            *(u8 *)(p + 5) = 1;
            *(u16 *)(q + 0x50) = 1;
        }
    } else if (*(u8 *)(base + 0xc1) == 1 || *(u8 *)(base + 0xc3) == 0) {
        if (*(u16 *)(base + 2) & 0x20)
            *(u8 *)(p + 1) = 0;
        else if (*(u16 *)(base + 2) & 0x10)
            *(u8 *)(p + 1) = 1;
    }
    func_ov002_02086f14();
}
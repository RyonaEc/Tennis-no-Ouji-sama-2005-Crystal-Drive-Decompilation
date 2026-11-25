#include "types.h"

extern u32 data_ov003_020eb720[];
extern u32 func_ov003_0204cf6c(void);
extern void func_ov003_0204d418(u32 a0, u32 a1);
extern void func_ov003_0204db9c(void);

void func_ov003_020d5948(u32 a0, u32 a1)
{
    u32 base = data_ov003_020eb720[0];
    u8 *p = (u8 *)(base + 0xb8dc);
    u8 *q = (u8 *)(base + 0xc8dc);

    if (a1 != 0)
        *(u16 *)(q + 4) = 0x10;
    switch (a1) {
    case 8:
    case 9:
        func_ov003_0204d418(*(u16 *)(p + 6), a0);
        func_ov003_0204db9c();
        break;
    case 7:
        func_ov003_0204d418(*(u16 *)(p + 6), a0);
        func_ov003_0204db9c();
        break;
    case 6:
        func_ov003_0204cf6c();
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        break;
    }
}
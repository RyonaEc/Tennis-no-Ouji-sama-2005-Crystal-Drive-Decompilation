#include "types.h"

extern u32 data_ov003_020eb720[];

extern u32 func_02020d44(u32 a0);
extern u32 func_02020c50(u32 a0);
extern void func_ov003_0205e3c0(void);
extern u32 func_020245f4(void);
extern u32 func_02024428(void);
extern void func_020243e0(void);
extern u32 func_02024314(void);
extern void func_ov003_020542e0(u32 a0, u32 a1);

u32 func_ov003_020d5d10(u32 a0)
{
    u32 ip = data_ov003_020eb720[0];

    if (a0 == 0) {
        func_02020d44(1);
        func_02020c50(0x40);
        if (*(u16 *)(ip + 0x8700 + 0x18) < 0x1f4)
            func_ov003_0205e3c0();
        func_020245f4();
        func_02024428();
        func_020243e0();
        func_02024314();
        *(u16 *)0x04000008 = (*(u16 *)0x04000008 & ~3) | 2;
    } else {
        *(u16 *)(ip + 0x8900 + 0xe4) = 0;
        *(u16 *)(ip + 0x8900 + 0xe0) = 0x10;
        *(u16 *)(ip + 0x8900 + 0xe2) = 0;
        func_ov003_020542e0(4, ip + 0xc8dc);
    }
    return 0;
}
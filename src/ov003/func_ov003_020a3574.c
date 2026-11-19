#include "types.h"

extern void func_020243e0(void);
extern void func_02024314(void);
extern void func_020245f4(void);
extern void func_02024428(void);
extern void func_02025c94(void *a0, u32 a1);
extern void func_0202436c(void *a0, u32 a1, u32 a2);
extern void func_02024498(void *a0, u32 a1, u32 a2);
extern u32 func_ov003_02055744(u32 a0, u8 *a1);
extern u32 func_ov003_0205578c(u32 a0, u8 *a1);
extern u32 func_ov003_0204d814(u32 a0, u32 a1);
extern void func_ov003_0204d9e4(u32 a0, u32 a1, u32 a2);

extern u8 data_ov003_020eb720[];
extern u8 data_ov003_020eb724[];

void func_ov003_020a3574(u32 a0, u32 a1)
{
    u8 *p = *(u8 **)data_ov003_020eb724;
    u8 *q = *(u8 **)data_ov003_020eb720;
    u8 *hi = p + 0x12c200;
    u8 *lo = p + 0x12c000;

    switch (a1) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        break;
    case 8:
    case 9:
        func_ov003_0204d9e4(1, 0, func_ov003_0204d814(1, a0));
        break;
    case 7:
        func_020243e0();
        func_02025c94(lo, 0x200);
        func_0202436c(lo, func_ov003_02055744(0x13, q + 0xb7dc), 0x200);
        func_02024314();
        func_020245f4();
        func_02025c94(hi, 0x1000);
        func_02024498(hi, func_ov003_0205578c(0x13, q + 0xb6dc), 0x1000);
        func_02024428();
        break;
    }
}
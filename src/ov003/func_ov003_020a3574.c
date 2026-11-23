#include "types.h"

extern u8 data_ov003_020eb724[];
extern u8 data_ov003_020eb720[];
extern void func_020243e0(void);
extern void func_02025c94(u8 *, s32);
extern void func_0202436c(u8 *, s32, s32);
extern void func_02024314(void);
extern void func_020245f4(void);
extern void func_02024498(u8 *, s32, s32);
extern void func_02024428(void);
extern s32 func_ov003_02055744(s32, u8 *);
extern s32 func_ov003_0205578c(s32, u8 *);
extern s32 func_ov003_0204d814(s32, s32);
extern void func_ov003_0204d9e4(s32, s32, s32);

void func_ov003_020a3574(s32 a0, s32 a1)
{
    u8 *base = *(u8 **)data_ov003_020eb724;
    u8 *r = *(u8 **)data_ov003_020eb720;
    u8 *p = base + 0x12c200;
    u8 *q = base + 0x12c000;

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
        func_02025c94(q, 0x200);
        func_0202436c(q, func_ov003_02055744(0x13, r + 0xb7dc), 0x200);
        func_02024314();
        func_020245f4();
        func_02025c94(p, 0x1000);
        func_02024498(p, func_ov003_0205578c(0x13, r + 0xb6dc), 0x1000);
        func_02024428();
        break;
    }
}
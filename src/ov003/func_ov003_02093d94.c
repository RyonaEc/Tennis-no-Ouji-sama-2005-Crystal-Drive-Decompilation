#include "types.h"

extern u8 data_ov003_020eb724[];
extern s32 func_02008150(s32);
extern void func_020243e0(void);
extern void func_02025c94(u8 *, s32);
extern void func_0202436c(u8 *, s32, s32);
extern void func_02024314(void);
extern void func_020245f4(void);
extern void func_02024498(u8 *, s32, s32);
extern void func_02024428(void);
extern s32 func_ov003_0204d814(s32, s32);
extern void func_ov003_0204d9e4(s32, s32, s32);

void func_ov003_02093d94(s32 a0)
{
    u8 *base = *(u8 **)data_ov003_020eb724;
    u8 *p = base + 0x124200;
    u8 *q = base + 0x124000;
    s32 v = func_02008150(a0);

    switch (a0) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        break;
    case 8:
        func_ov003_0204d9e4(0, 0, func_ov003_0204d814(0, v));
        break;
    case 6:
    case 7:
        break;
    case 5:
        func_020243e0();
        func_02025c94(q, 0x200);
        func_0202436c(q, 0x20, 0x200);
        func_02024314();
        func_020245f4();
        func_02025c94(p, 0x4000);
        func_02024498(p, 0x2000, 0x4000);
        func_02024428();
        break;
    case 9:
        break;
    }
}
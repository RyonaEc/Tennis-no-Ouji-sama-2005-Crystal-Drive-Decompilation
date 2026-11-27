#include "types.h"

extern u8 *data_ov003_020eb724;
extern u8 *data_ov003_020eb720;

void func_020243e0(void);
void func_02025c94(u32 a0, u32 a1);
void func_0202436c(u32 a0, u32 a1, u32 a2);
u32 func_02024314(void);
u32 func_020245f4(void);
u32 func_02024498(u32 a0, u32 a1, u32 a2);
u32 func_02024428(void);
u32 func_ov003_02055744(u32 a0, u32 *a1);
s32 func_ov003_0205578c(u32 a0, u32 *a1);

void func_ov003_02066f48(u32 a, u32 sel) {
    u8 *base = data_ov003_020eb724;
    u8 *g = data_ov003_020eb720;
    u32 p1 = (u32)(base + 0x124200);
    u32 p0 = (u32)(base + 0x124000);

    switch (sel) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        break;
    case 6:
        func_020243e0();
        func_02025c94(p0, 0x200);
        func_0202436c(p0, func_ov003_02055744(2, (u32 *)(g + 0xb7dc)), 0x200);
        func_02024314();
        func_020245f4();
        func_02025c94(p1, 0x1000);
        func_02024498(p1, func_ov003_0205578c(2, (u32 *)(g + 0xb6dc)), 0x1000);
        func_02024428();
        break;
    case 7:
    case 8:
    case 9:
        break;
    }
}
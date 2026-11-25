#include "types.h"

extern u32 data_ov003_020eb720[];
extern u32 data_ov003_020eb724[];

extern u32 func_020245f4(void);
extern void func_02025c94(u32 a0, u32 a1);
extern u32 func_02024498(u32 a0, u32 a1, u32 a2);
extern u32 func_02024428(void);
extern void func_020243e0(void);
extern void func_0202436c(u32 a0, u32 a1, u32 a2);
extern u32 func_02024314(void);

void func_ov003_0205e3c0(void)
{
    u32 a = data_ov003_020eb720[0];
    u32 b = data_ov003_020eb724[0];

    func_020245f4();
    func_02025c94(b + 0x1b340, 0x2000);
    func_02024498(b + 0x1b340, *(u32 *)(a + 0x20), 0x2000);
    *(u32 *)(a + 0x20) = 0x2000;
    func_02024428();
    func_020243e0();
    func_02025c94(b + 0x1b320, 0x20);
    func_0202436c(b + 0x1b320, *(u32 *)(a + 0x24), 0x20);
    *(u32 *)(a + 0x24) = 0x20;
    func_02024314();
}
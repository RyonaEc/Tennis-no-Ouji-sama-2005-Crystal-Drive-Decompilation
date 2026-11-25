#include "types.h"

extern u32 data_ov005_020c9740[];
extern u32 data_ov005_020c9744[];

extern u32 func_020245f4(void);
extern void func_02025c94(void *a0, u32 a1);
extern u32 func_02024498(void *a0, u32 a1, u32 a2);
extern u32 func_02024428(void);
extern void func_020243e0(void);
extern void func_0202436c(void *a0, u32 a1, u32 a2);
extern u32 func_02024314(void);

void func_ov005_0205526c(void)
{
    u8 *a = (u8 *)data_ov005_020c9740[0];
    u8 *b = (u8 *)data_ov005_020c9744[0];

    func_020245f4();
    func_02025c94(b + 0x20820, 0x2000);
    func_02024498(b + 0x20820, *(u32 *)(a + 0x20), 0x2000);
    *(u32 *)(a + 0x20) = 0x2000;
    func_02024428();
    func_020243e0();
    func_02025c94(b + 0x20800, 0x20);
    func_0202436c(b + 0x20800, *(u32 *)(a + 0x24), 0x20);
    *(u32 *)(a + 0x24) = 0x20;
    func_02024314();
}
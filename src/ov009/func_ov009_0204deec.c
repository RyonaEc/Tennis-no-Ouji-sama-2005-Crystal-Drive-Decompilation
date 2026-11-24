#include "types.h"

extern u8 *data_ov009_02054f84;
extern u8 *data_ov009_02054f80;
extern u32 func_020245f4(void);
extern void func_02025c94(u32 a0, u32 a1);
extern u32 func_02024498(u32, u32, u32);
extern u32 func_02024428(void);
extern void func_020243e0(void);
extern void func_0202436c(u32 a0, u32 a1, u32 a2);
extern u32 func_02024314(void);

void func_ov009_0204deec(void)
{
    u8 *s = data_ov009_02054f84;
    u32 t = (u32)data_ov009_02054f80;

    func_020245f4();
    func_02025c94((t + 0x1e4c0), 0x2000);
    func_02024498((t + 0x1e4c0), *(u32 *)(s + 0x20), 0x2000);
    *(u32 *)(s + 0x20) = 0x2000;
    func_02024428();
    func_020243e0();
    func_02025c94((t + 0x1e4a0), 0x20);
    func_0202436c((t + 0x1e4a0), *(u32 *)(s + 0x24), 0x20);
    *(u32 *)(s + 0x24) = 0x20;
    func_02024314();
}
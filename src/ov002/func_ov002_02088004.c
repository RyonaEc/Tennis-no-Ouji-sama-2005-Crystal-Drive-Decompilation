#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern void func_0203447c(u8 *, u32, u32);
extern void func_ov002_02086020(u32);
extern void func_ov002_02087d64(void);
void func_ov002_02088004(void)
{
    u8 *b = B;
    func_0203447c(b + 0x47d0, 0, 0x3ec);
    if (*(u8 *)(b + 0xc2) == 1)
        func_ov002_02086020(2);
    func_ov002_02087d64();
}

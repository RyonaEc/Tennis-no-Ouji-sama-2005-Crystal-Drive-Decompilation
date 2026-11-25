#include "types.h"

extern u8 data_027c0000;
extern void func_ov008_0204caa8(void);
extern void func_ov008_0204e7ac(void);

void func_ov008_0204e51c(void)
{
    func_ov008_0204caa8();
    func_ov008_0204e7ac();
    *(u32 *)((u8 *)&data_027c0000 + 0x3ff8) |= 1;
}
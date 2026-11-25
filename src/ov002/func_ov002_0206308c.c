#include "types.h"

extern u8 data_027c0000;
extern void func_ov002_0204efd8(void);
extern void func_ov002_020842f8(void);

void func_ov002_0206308c(void)
{
    func_ov002_0204efd8();
    func_ov002_020842f8();
    *(u32 *)((u8 *)&data_027c0000 + 0x3ff8) |= 1;
}
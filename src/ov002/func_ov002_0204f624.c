#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_0203447c(u32, u32, u32);
void func_ov002_0204f624(void)
{
    func_0203447c(*(u32 *)data_ov002_02102070 + 0x1a60a8, 0, 0x1e28);
}

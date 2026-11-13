#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_0203447c(u32, u32, u32);
void func_ov002_0204d464(void)
{
    func_0203447c(*(u32 *)data_ov002_02102070 + 0x274, 0, 0x70);
}

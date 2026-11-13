#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_0203447c(u32, u32, u32);
void func_ov002_02067acc(void)
{
    func_0203447c(*(u32 *)data_ov002_02102070 + 0x1744, 0, 0xe0);
}

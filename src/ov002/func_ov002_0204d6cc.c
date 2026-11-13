#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_02022144(u32, u32);
void func_ov002_0204d6cc(void)
{
    func_02022144(*(u32 *)data_ov002_02102070 + 0x1a89a4, 1);
}

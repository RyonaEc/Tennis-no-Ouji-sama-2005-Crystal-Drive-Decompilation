#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_0203447c(u8 *, u32, u32);
void func_ov002_02081dcc(void)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x3654);
    u16 i;
    func_0203447c(p, 0, 0x58);
    for (i = 0; i < 4; i++)
        *(u8 *)(p + 9 + i) = 0;
}

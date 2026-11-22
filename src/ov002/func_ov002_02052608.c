#include "types.h"

extern u8 data_ov002_02102070;
extern void func_ov002_0207808c(void);

void func_ov002_02052608(void)
{
    u32 *p = (u32 *)(*(u8 **)&data_ov002_02102070 + 0x1824);
    u32 *q = (u32 *)((u8 *)p + 0xb8);

    *q = *q & 0xfffa3c0f;
    *q = *q | 0x800;
    func_ov002_0207808c();
}
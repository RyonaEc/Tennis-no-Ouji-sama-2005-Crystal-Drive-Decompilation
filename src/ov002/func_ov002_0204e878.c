#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 *func_ov002_020848d0(void);
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_0204e878(void)
{
    u8 *b = B + 0x1e34;
    u8 *p = func_ov002_020848d0();
    if (!(*(u32 *)(b + 0x34) & 1)) return;
    *(u32 *)p |= 0x800;
    *(u16 *)(p + 0x20) = *(u16 *)(b + 2) & 0x1ff;
    *(u16 *)(p + 0x22) = *(u16 *)(b + 4) & 0x1ff;
    *(u16 *)(p + 0x12) = *(u16 *)(b + 0x20);
}

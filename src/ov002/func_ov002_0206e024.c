#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern void func_ov002_0204f5a0(u32, u32, u32);
void func_ov002_0206e024(u8 a0)
{
    u8 *p = B + 0x1e6c;
    *(u8 *)(p + 0x618) = a0;
    *(u8 *)(p + 0x619) = 1;
    *(u32 *)(p + 0x61c) = 0;
    func_ov002_0204f5a0(*(u32 *)(B + 0x4000 + 0xf4c), 0x6019000, 0x20);
}

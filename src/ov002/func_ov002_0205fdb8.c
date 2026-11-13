#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_0205fdb8(u32 a0)
{
    u8 *b = B;
    u8 v = *(u8 *)(b + a0 + 0x4000 + 0xbc4);
    u8 w = *(u8 *)(b + a0 + 0x22a);
    u8 k = *(u8 *)(b + v * 3 + w + 0x224);
    *(u8 *)(b + v * 3 + k + 0x21e) = 1;
}

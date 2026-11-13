#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_02062d84(u32 *o, u8 *o2)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    *o = *(u32 *)(b + 8);
    *o2 = *(u8 *)(b + 0xc);
}

#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_020841e8(void)
{
    u8 *p = B + 0x36ac;
    *(u8 *)(p + 5) = 0;
    *(u8 *)(p + 6) = 0;
}

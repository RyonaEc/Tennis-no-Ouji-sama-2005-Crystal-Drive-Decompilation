#include "types.h"

extern u8 data_ov008_0204f440[];

void func_ov008_0204cf5c(void)
{
    u8 *p = *(u8 **)data_ov008_0204f440;

    *(u8 *)(p + 0x31) = 0;
    *(s8 *)(p + 0x32) = -1;
    *(u8 *)(p + 0x33) = 0;
}
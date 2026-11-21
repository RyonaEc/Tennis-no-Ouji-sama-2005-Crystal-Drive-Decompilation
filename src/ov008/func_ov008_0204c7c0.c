#include "types.h"

extern u8 *data_ov008_0204f440;

u16 func_ov008_0204c7c0(void)
{
    u8 *p = data_ov008_0204f440 + 0x2576c;
    u16 v = *(u16 *)(p + 0x20030);

    if (v >= 0x1f)
        *(u16 *)(p + 0x20030) = 0;
    else
        (*(u16 *)(p + 0x20030))++;
    return *(u16 *)(p + 0x20030);
}
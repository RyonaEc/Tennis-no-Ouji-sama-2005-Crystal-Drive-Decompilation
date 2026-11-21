#include "types.h"

u16 func_ov008_0204c7c0(void)
{
    u8 *p = *(u8 **)0x0204f440 + 0x2576c;
    u16 v = *(u16 *)(p + 0x20030);

    if (v >= 0x1f)
        *(u16 *)(p + 0x20030) = 0;
    else
        (*(u16 *)(p + 0x20030))++;
    return *(u16 *)(p + 0x20030);
}
#include "types.h"

extern u32 data_ov008_0204f440;

void func_ov008_0204d008(void)
{
    *(u8 *)(data_ov008_0204f440 + 0x30) = 0;
}

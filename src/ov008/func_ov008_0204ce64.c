#include "types.h"

extern u32 data_ov008_0204f440;

s32 func_ov008_0204ce64(void)
{
    return (*(u32 *)(data_ov008_0204f440 + 0x3c) & 0x200) != 0;
}

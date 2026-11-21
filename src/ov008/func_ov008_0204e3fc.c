#include "types.h"

extern u32 data_ov008_0204f440;

void func_ov008_0204e3fc(void)
{
    if (*(u16 *)(data_ov008_0204f440 + 4) == 0)
        func_ov008_0204e13c();
}

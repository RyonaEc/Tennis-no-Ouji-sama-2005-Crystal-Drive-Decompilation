#include "types.h"

extern u8 data_ov008_0204f440[];
extern void func_0203447c(u8 *, s32, s32);

void func_ov008_0204ca78(void)
{
    u8 *base = *(u8 **)data_ov008_0204f440;

    func_0203447c(base + 0x2576c, 0, 0x24850);
}
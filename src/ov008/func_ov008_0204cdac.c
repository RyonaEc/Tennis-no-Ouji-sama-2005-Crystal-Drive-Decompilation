#include "types.h"

extern u8 data_ov008_0204f440[];
extern void func_0203447c(s32 a0, s32 a1, s32 a2);

void func_ov008_0204cdac(void)
{
    func_0203447c(*(s32 *)data_ov008_0204f440 + 0x24554, 0, 0x1218);
}

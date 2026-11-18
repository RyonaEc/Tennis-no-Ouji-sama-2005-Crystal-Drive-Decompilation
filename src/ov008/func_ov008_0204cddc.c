#include "types.h"

extern u8 data_ov008_0204f440[];
extern u8 data_ov008_0204f2c0[];
extern s32 func_02006448(void);
extern void func_0200649c(u32);

void func_ov008_0204cddc(void)
{
    u16 v = *(u16 *)(data_ov008_0204f2c0 +
                     (*(u8 *)(*(u8 **)data_ov008_0204f440 + 0x21) << 1));

    if (func_02006448() == 1)
        return;
    func_0200649c(v);
}
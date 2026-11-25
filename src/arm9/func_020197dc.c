#include "types.h"

extern u32 data_02042360;
extern u32 data_02042358;
extern u32 data_02042364;
extern u16 data_02042354;
extern u8 data_02046b34[];
extern u16 func_0201a04c(void);

void func_020197dc(void)
{
    u16 v;

    func_0201a04c();
    if (data_02042360 > (u32)data_02046b34) {
        data_02042360 -= 0x18;
        data_02042358 -= 0x18;
        data_02042364 -= 4;
    }
    if (data_02042354 == 0xfffe)
        return;
    v = func_0201a04c();
    if (data_02042354 > v)
        data_02042354 = 0xfffe;
}
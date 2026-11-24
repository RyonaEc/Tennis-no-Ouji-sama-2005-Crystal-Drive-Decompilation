#include "types.h"

extern u8 *data_ov006_02077e40;
extern u32 func_02026a04(u32, u32, u32);

void func_ov006_0204e3b8(void)
{
    volatile u32 s0;
    volatile u32 s1;
    u32 j;

    s0 = 0xc0;
    j = *(u32 *)(data_ov006_02077e40 + 0x11c4);
    func_02026a04(s0, (u32)(data_ov006_02077e40 + 0x11d8 + (j << 10)), 0x400);
    s1 = 0xc0;
    func_02026a04(s1, (u32)(data_ov006_02077e40 + 0x19d8 + (j << 10)), 0x400);
    *(u32 *)(data_ov006_02077e40 + 0x11d0 + j * 4) = 0;
    *(u32 *)(data_ov006_02077e40 + 0x11c8 + j * 4) = *(u32 *)(data_ov006_02077e40 + 0x11d0 + j * 4);
}
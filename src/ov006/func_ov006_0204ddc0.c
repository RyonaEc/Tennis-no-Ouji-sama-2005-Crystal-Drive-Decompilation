#include "types.h"

extern u32 data_ov006_02077c4c[];
extern u32 data_ov006_02077cdc[];

extern void func_020208b0(u32 sel);
extern u32 func_02008150(void);
extern void func_02025c94(u32, u32);
extern u32 func_02023df8(u32, u32, u32);
extern u32 func_020233cc(void);
extern u32 func_02023358(u32, u32, u32);
extern u32 func_0202330c(u32);

void func_ov006_0204ddc0(void)
{
    u32 i;

    func_020208b0(0x100);
    i = func_02008150();
    func_02025c94(data_ov006_02077c4c[i], 0x1000);
    func_02023df8(data_ov006_02077c4c[i], 0x4000, 0x1000);
    func_02025c94(data_ov006_02077cdc[i], 0x200);
    func_020233cc();
    func_0202330c(func_02023358(data_ov006_02077cdc[i], 0, 0x200));
}
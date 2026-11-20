#include "types.h"

extern void func_02025c94(u32 a0, u32 a1);
extern void func_02023df8(u32 a0, u32 a1, u32 a2);

s32 func_ov003_02050460(void)
{
    func_02025c94(0x020e6fe4, 0x800);
    func_02023df8(0x020e6fe4, 0xe740, 0x800);
    func_02025c94(0x020e6fe4, 0x800);
    func_02023df8(0x020e6fe4, 0xef40, 0x800);
    return 0;
}
#include "types.h"

extern void func_02025c94(u8 *p, u32 n);
extern void func_02023df8(u8 *p, u32 a, u32 b);
extern u8 data_ov003_020e6fe4[];

s32 func_ov003_02050460(void)
{
    func_02025c94(data_ov003_020e6fe4, 0x800);
    func_02023df8(data_ov003_020e6fe4, 0xe740, 0x800);
    func_02025c94(data_ov003_020e6fe4, 0x800);
    func_02023df8(data_ov003_020e6fe4, 0xef40, 0x800);
    return 0;
}

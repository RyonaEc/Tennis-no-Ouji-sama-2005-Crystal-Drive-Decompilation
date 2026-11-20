#include "types.h"

extern void func_02023df8(u8 *a0, u32 a1, u32 a2);

void func_ov003_0204e5bc(void)
{
    u8 *p = (u8 *)0x20e6fe4;

    func_02023df8(p, 0xe740, 0x800);
    func_02023df8(p, 0xef40, 0x800);
}
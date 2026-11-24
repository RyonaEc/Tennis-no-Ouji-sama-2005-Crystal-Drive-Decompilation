#include "types.h"

extern u8 *data_ov010_0205e500;
extern void func_ov010_0204f5f8(u32 x);
extern void func_02021af0(u8 *a0, u16 a1, s32 a2);

void func_ov010_0204f514(void)
{
    u8 *p = data_ov010_0205e500;

    if (p[0x3b0] == 0)
        return;
    p[0x3b0] -= 1;
    p = data_ov010_0205e500;
    if (p[0x3b0] == 0x10) {
        func_ov010_0204f5f8(p[0x3b2]);
        p = data_ov010_0205e500;
        p[0x3b1] = p[0x3b2];
        return;
    }
    func_02021af0((u8 *)0x04001050, 4, -(s32)p[0x3b0]);
}
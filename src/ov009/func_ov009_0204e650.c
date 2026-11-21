#include "types.h"

extern void func_ov009_0204dc48(void);
extern void func_ov009_0204e738(s32 a0, s32 a1);
extern void func_ov009_0204e7d4(u32 a0);
extern void func_ov009_0204e7f0(s32 a0, s32 a1);

void func_ov009_0204e650(void)
{
    func_ov009_0204dc48();
    *(u32 *)0x040004a8 = 0x6e400000;
    *(u32 *)0x040004ac = 0;
    *(u32 *)0x040004a4 = 0x1f00c0;
    *(u32 *)0x04000500 = 1;
    func_ov009_0204e738(1, 0);
    func_ov009_0204e7d4(0);
    func_ov009_0204e7f0(2, 0);
    func_ov009_0204e738(0, 0);
    func_ov009_0204e7d4(0);
    func_ov009_0204e7f0(0, 0);
    func_ov009_0204e738(2, 0);
    func_ov009_0204e7d4(0);
    func_ov009_0204e7f0(4, 0);
    func_ov009_0204e738(3, 0);
    func_ov009_0204e7d4(0);
    func_ov009_0204e7f0(6, 0);
    *(u32 *)0x04000504 = 0;
}
#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 func_ov002_020858cc(u32);
extern void func_ov002_02085940(u32);
extern void func_ov002_02080b70(void);
extern void func_ov002_02084ff4(void);
extern void func_ov002_020589d0(u32);
void func_ov002_02085468(u32 a0, u32 a1)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    u8 *p = b + 0x158 + a0 * 0xc;
    *p = func_ov002_020858cc(*p - a1);
    func_ov002_02085940(a0);
    func_ov002_02080b70();
    func_ov002_02084ff4();
    func_ov002_020589d0(a0);
}

#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_02085940(u32);
extern void func_ov002_02080b70(void);
extern void func_ov002_02084ff4(void);
void func_ov002_02084f70(u32 a0, u8 a1)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    *(u8 *)(b + a0 + 0x66) = a1;
    func_ov002_02085940(a0 & 0xff);
    func_ov002_02080b70();
    func_ov002_02084ff4();
}

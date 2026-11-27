#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_02085940(s32 a0);
extern void func_ov002_02080b70(void);
extern void func_ov002_02084ff4(void);
extern void func_ov002_020589d0(s32 a0);

void func_ov002_020852e0(s32 a0, s32 a1)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *e = p + 0x156 + a0 * 0xc;

    if (a1 & 0x1)
        e[0] = 1;
    if (a1 & 0x2)
        e[1] = 1;
    if (a1 & 0x4)
        e[2] = 1;
    if (a1 & 0x8)
        e[3] = 1;
    if (a1 & 0x10)
        e[4] = 1;
    if (a1 & 0x20)
        e[5] = 1;
    if (a1 & 0x40)
        e[6] = 1;
    if (a1 & 0x80)
        e[7] = 1;
    if (a1 & 0x100)
        e[8] = 1;
    if (a1 & 0x200)
        e[9] = 1;
    if (a1 & 0x400)
        e[10] = 1;
    if (a1 & 0x800)
        e[11] = 1;
    func_ov002_02085940(a0);
    func_ov002_02080b70();
    func_ov002_02084ff4();
    func_ov002_020589d0(a0);
}
#include "types.h"

extern u8 data_ov002_02102070[];
extern s32 func_ov002_020858cc(s32 a0);
extern void func_ov002_02085940(s32 a0);
extern void func_ov002_02080b70(void);
extern void func_ov002_02084ff4(void);

void func_ov002_02085098(s32 a0, u8 *a1)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *e = p + 0x156 + a0 * 0xc;

    e[0] = func_ov002_020858cc(a1[0]);
    e[1] = func_ov002_020858cc(a1[1]);
    e[2] = func_ov002_020858cc(a1[2]);
    e[3] = func_ov002_020858cc(a1[3]);
    e[4] = func_ov002_020858cc(a1[4]);
    e[5] = func_ov002_020858cc(a1[5]);
    e[6] = func_ov002_020858cc(a1[6]);
    e[7] = func_ov002_020858cc(a1[7]);
    e[8] = func_ov002_020858cc(a1[8]);
    e[9] = func_ov002_020858cc(a1[9]);
    e[10] = func_ov002_020858cc(a1[10]);
    e[11] = func_ov002_020858cc(a1[11]);
    func_ov002_02085940(a0);
    func_ov002_02080b70();
    func_ov002_02084ff4();
}
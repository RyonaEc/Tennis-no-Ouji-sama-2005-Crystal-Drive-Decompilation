#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_020067fc(void);
extern s32 func_020067dc(s32 v);
extern u32 func_0203447c(u32 a0, u32 a1, u32 a2);
extern u8 *func_02034494(u8 *d, u8 *s, u32 n);
extern u32 func_02005958(u32, u32);
extern void func_ov002_02060688(u32 a0, u32 a1, u32 a2);
extern void func_020064c0(s32 sel, s32 arg);

void func_ov002_020870fc(void)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *q;

    p[0x24] = 1;
    q = p + 0x47d0;
    func_020067fc();
    func_020067dc(0x7f);
    func_0203447c((u32)((u8 *)q + 0x1ec), 0, 0x200);
    func_02034494(q + 0x1ec, (u8 *)((u32)q + 0x18), 0x1d4);
    switch (func_02005958(3u, (u32)q + 0x1ec)) {
    case 0:
        func_ov002_02060688(7, 1, 1);
        return;
    case 1:
        func_ov002_02060688(7, 0, 1);
        func_020064c0(0, 0x2f0);
        return;
    default:
        func_ov002_02060688(0, 0, 1);
        return;
    }
}
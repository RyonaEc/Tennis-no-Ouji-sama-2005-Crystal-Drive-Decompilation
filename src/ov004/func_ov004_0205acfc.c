#include "types.h"

extern u32 data_ov004_020690a0;

extern void func_ov004_02053518(u8 *);
extern u8 *func_ov004_0204e8a4(s32, s32);
extern void func_02026af4(u8 *, s32, s32);

void func_ov004_0205acfc(void)
{
    u8 *g = (u8 *)data_ov004_020690a0;
    u32 found = 0;
    s32 i;
    u8 *p;
    u8 *r;

    g = (u8 *)data_ov004_020690a0;
    found = 0;
    p = (u8 *)((u32)g + 0xd24);
    p = (u8 *)((u32)p + 0x1b4);
    p = p + 0x110;
    for (i = 2; i >= 0; i--) {
        if (p != 0) {
            if (*(u32 *)p != 0)
                found = 1;
            func_ov004_02053518(p);
        }
        p -= 0x88;
    }
    if (found == 0)
        return;
    r = func_ov004_0204e8a4(0, 2);
    if (r == 0)
        return;
    func_02026af4(r, 0, 0x88);
}
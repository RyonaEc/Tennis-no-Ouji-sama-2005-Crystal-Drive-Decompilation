#include "types.h"

extern s32 func_ov004_0206256c(u32);
extern s32 func_020101b4(u32);
extern void func_ov004_020624e0(u32, u32);

void func_ov004_02062380(void)
{
    s32 i;
    s32 r;

    if (func_ov004_0206256c(1) != 0)
        return;
    for (i = 0; i < 0x1b;) {
        r = func_020101b4(i);
        i = i + 1;
        if (r == 2 || r == 3)
            continue;
        if (r != 4)
            return;
    }
    func_ov004_020624e0(1, 1);
}
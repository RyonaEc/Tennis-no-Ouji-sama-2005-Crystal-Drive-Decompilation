#include "types.h"

extern void func_02017db8(u8 *a0, s32 a1, s32 a2);
extern void func_02017cf4(u8 *a0, s32 a1, s32 a2);

void func_02017b60(u8 *a0, s32 a1, s32 a2)
{
    if (!a1)
        a1 = 1;
    a1 = (a1 + 3) & ~3;
    if (a2 >= 0)
        func_02017db8(a0, a1, a2);
    else
        func_02017cf4(a0, a1, -a2);
}

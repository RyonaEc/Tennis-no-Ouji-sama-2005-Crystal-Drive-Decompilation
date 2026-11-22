#include "types.h"

extern s32 func_ov003_0204d814(u32 a0, u32 a1);
extern void func_ov003_0204d9e4(u32 a0, u32 a1, u32 a2);

void func_ov003_02067020(u32 a0, u32 a1)
{
    s32 t;

    switch (a1) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        break;
    case 8:
    case 9:
        t = func_ov003_0204d814(1, a0);
        func_ov003_0204d9e4(0, 0, t);
        break;
    }
}
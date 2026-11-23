#include "types.h"

extern s32 func_ov003_0204d814(s32, s32);
extern void func_ov003_0204d9e4(s32, s32, s32);

void func_ov003_02067020(s32 a0, s32 a1)
{
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
        func_ov003_0204d9e4(0, 0, func_ov003_0204d814(1, a0));
        break;
    }
}
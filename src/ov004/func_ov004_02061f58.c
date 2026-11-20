#include "types.h"

extern u32 data_ov004_020690a8;
extern s32 func_ov004_0206256c(u32 a0);

s32 func_ov004_02061f58(void)
{
    u32 v = *(u32 *)(data_ov004_020690a8 + 8);

    switch (v) {
    case 0:
        return func_ov004_0206256c(2) != 0;
    case 1:
        return 2;
    default:
        return 3;
    }
}
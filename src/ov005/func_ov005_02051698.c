#include "types.h"

extern u8 *data_ov005_020c9740;

extern u32 func_ov005_0204d52c(void);

void func_ov005_02051698(u32 a0)
{
    u8 *g = data_ov005_020c9740;

    switch (a0) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 7:
    case 8:
    case 9:
        break;
    case 5:
        func_ov005_0204d52c();
        *(u16 *)(g + 0x437e) = *(u16 *)0x04000006;
        break;
    }
}
#include "types.h"

void func_ov004_02053218(u8 *p, s32 *a, s32 *b)
{
    *a = 1;
    *b = 1;
    switch (*(u32 *)(p + 0x84)) {
    case 1:
        *b = 0;
        break;
    case 2:
        *a = 0;
        break;
    case 3:
        *a = 0;
        *b = 0;
        break;
    }
}
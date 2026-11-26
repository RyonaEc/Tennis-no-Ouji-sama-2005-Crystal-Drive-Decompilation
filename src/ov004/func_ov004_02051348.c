#include "types.h"

extern s32 func_ov004_02051410(u8 *p);

s32 func_ov004_02051348(u8 *p)
{
    (*(s16 *)(p + 0xa4))++;
    if (*(s16 *)(p + 0x9c) >= 0) {
        (*(s16 *)(p + 0xa2))++;
        if (*(s16 *)(p + 0xa2) > *(s16 *)(p + 0x9c)) {
            *(u32 *)(p + 0x70) = 1;
            func_ov004_02051410(p);
            return 1;
        }
    }
    switch (*(s16 *)(p + 0xa6)) {
    case 0:
        *(u32 *)(p + 0x70) = 1;
        if (*(s16 *)(p + 0xa4) > *(s16 *)(p + 0x9e)) {
            *(s16 *)(p + 0xa4) = 0;
            *(s16 *)(p + 0xa6) = 1;
        }
        break;
    case 1:
        *(u32 *)(p + 0x70) = 0;
        if (*(s16 *)(p + 0xa4) > *(s16 *)(p + 0xa0)) {
            *(s16 *)(p + 0xa4) = 0;
            *(s16 *)(p + 0xa6) = 0;
        }
        break;
    }
    return 0;
}
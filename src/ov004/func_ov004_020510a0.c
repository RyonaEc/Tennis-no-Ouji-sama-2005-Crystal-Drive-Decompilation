#include "types.h"

extern void func_ov004_02053014(u32 a0, u32 a1, u8 *p);
extern void func_ov004_0205312c(u8 *p);
extern s32 func_ov004_02051410(u8 *p);

s32 func_ov004_020510a0(u8 *p)
{
    (*(s16 *)(p + 0xa2))++;
    switch (*(s16 *)(p + 0xa4)) {
    case 0:
        if (*(s16 *)(p + 0xa2) > *(s16 *)(p + 0xa0)) {
            (*(s16 *)(p + 0xa6))++;
            if (*(s16 *)(p + 0xa6) >= (*(s16 *)(p + 0x9c) + *(s16 *)(p + 0x9e)))
                *(s16 *)(p + 0xa6) = *(s16 *)(p + 0x9c);
            func_ov004_02053014(*(s16 *)(p + 0xa6), 1, p);
            *(s16 *)(p + 0xa2) = 0;
        }
        break;
    case 1:
        func_ov004_0205312c(p);
        func_ov004_02051410(p);
        return 1;
    }
    return 0;
}
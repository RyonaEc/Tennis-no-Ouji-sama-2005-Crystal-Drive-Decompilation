#include "types.h"

extern u8 *data_ov005_020c9740;
extern void func_ov005_0206f390(u32 a0);

u32 func_ov005_0206d778(void)
{
    u8 *b = data_ov005_020c9740;

    if (*(u16 *)(b + 0x43a6) < 0x10) {
        *(u16 *)(b + 0x36) = 2;
        func_ov005_0206f390(0);
    } else {
        *(u16 *)(b + 0x4332) = 0;
        *(u16 *)(b + 0x4330) = 0x44;
        *(u16 *)(b + 0x4388) = 0;
    }
    (*(u16 *)(b + 0x43a6))++;
    return 1;
}
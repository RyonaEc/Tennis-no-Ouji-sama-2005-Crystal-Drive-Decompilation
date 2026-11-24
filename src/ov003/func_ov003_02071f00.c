#include "types.h"

extern u32 data_ov003_020eb720;

u32 func_ov003_02071f00(u32 a0);

u32 func_ov003_02071f00(u32 a0)
{
    u8 *b;

    b = (u8 *)(data_ov003_020eb720 + 0xb8dc);

    if (a0 == 0) {
        if (*(u16 *)(b + 0xd0) < 0x21) {
            *(s16 *)(b + 0xd8) += 1;
            *(s16 *)(b + 0xda) += 1;
        }
        if (*(u16 *)(b + 0xd0) < 0x40) {
            if (*(s16 *)(b + 0xd8) <= 0x17) {
                if (*(u16 *)(b + 0xd4) < 0x20) {
                    *(u16 *)(b + 0xd4) += 1;
                    *(u16 *)(b + 0xd6) += 1;
                }
            } else {
                if (*(u16 *)(b + 0xd4) < 0x20) {
                    *(u16 *)(b + 0xd4) += 1;
                    *(u16 *)(b + 0xd6) += 1;
                }
            }
        }
    } else if (a0 == 1) {
        if (*(u16 *)(b + 0xd0) < 0x21) {
            *(s16 *)(b + 0xd8) += 1;
            *(s16 *)(b + 0xda) += 1;
        }
        if (*(u16 *)(b + 0xd0) < 0x40) {
            if (*(s16 *)(b + 0xd8) <= 0x17) {
                if (*(u16 *)(b + 0xd4) < 0x20) {
                    *(u16 *)(b + 0xd4) += 1;
                    *(u16 *)(b + 0xd6) += 1;
                }
            } else {
                if (*(u16 *)(b + 0xd4) < 0x20) {
                    *(u16 *)(b + 0xd4) += 1;
                    *(u16 *)(b + 0xd6) += 1;
                }
            }
        }
    }
    return 0;
}
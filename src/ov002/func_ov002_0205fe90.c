#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_0205fe90(u32 a0, u32 a1, u32 a2)
{
    u8 *b;
    u32 r2;
    u32 r0;

    b = *(u8 **)data_ov002_02102070;
    r0 = 0;
    if (a0 == 0) {
        switch (a2) {
        case 8:
            r2 = 0x100;
            break;
        case 0x10:
            r2 = 0x200;
            break;
        case 0x12:
            r2 = 0x400;
            break;
        case 6:
            r2 = 0x800;
            break;
        case 0xa:
            r2 = 0x1000;
            break;
        default:
            r2 = 0;
            break;
        }
    } else {
        switch (a2) {
        case 4:
            r2 = 1;
            break;
        case 0x32:
            r2 = 2;
            break;
        case 0x16:
            r2 = 4;
            break;
        case 6:
            r2 = 8;
            break;
        case 0x33:
            r2 = 0x10;
            break;
        case 0xa:
            r2 = 0x20;
            break;
        case 0x1b:
            r2 = 0x40;
            break;
        case 0x1a:
            r2 = 0x80;
            break;
        default:
            r2 = 0;
            break;
        }
    }
    if (r2 & (*(u32 *)(b + 0xf8 + a1 * 4)))
        r0 += 1;
    return r0;
}
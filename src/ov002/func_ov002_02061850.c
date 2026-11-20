#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_02061850(void)
{
    u8 *base;

    base = *(u8 **)data_ov002_02102070;
    if (*(u8 *)(base + 0x8c) == 0) {
        if (*(u8 *)(base + 0x113) != 0) {
            if (*(u8 *)(base + 0x113) == 1) {
                *(u8 *)(base + 0xac) = *(u8 *)(base + 0x10f);
                *(u8 *)(base + 0x113) = 0;
            }
        } else {
            *(u8 *)(base + 0xac) = *(u8 *)(base + 0x110);
            *(u8 *)(base + 0x113) = 1;
        }
        return;
    }
    switch (*(u8 *)(base + 0x113)) {
    case 0:
        *(u8 *)(base + 0xac) = *(u8 *)(base + 0x111);
        *(u8 *)(base + 0x113) = 2;
        break;
    case 1:
        *(u8 *)(base + 0xac) = *(u8 *)(base + 0x112);
        *(u8 *)(base + 0x113) = 3;
        break;
    case 2:
        *(u8 *)(base + 0xac) = *(u8 *)(base + 0x110);
        *(u8 *)(base + 0x113) = 1;
        break;
    case 3:
        *(u8 *)(base + 0xac) = *(u8 *)(base + 0x10f);
        *(u8 *)(base + 0x113) = 0;
        break;
    }
}
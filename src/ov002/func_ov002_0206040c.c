#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_0206040c(void)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    s32 i;

    for (i = 0; i < 4; i++) {
        switch (*(u8 *)(p + i + 0x152)) {
        case 0:
            break;
        case 1:
            *(u8 *)(p + i + 0x152) = 2;
            break;
        case 2:
            *(u8 *)(p + i + 0x152) = 0;
            break;
        }
    }
}

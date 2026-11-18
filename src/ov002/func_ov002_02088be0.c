#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_020882b8(u32, u32);

void func_ov002_02088be0(void)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *q = p + 0x4bc4;

    if (*(u8 *)(q + 0x35e) == 5) {
        *(u8 *)(q + 0x379) = 1;
        func_ov002_020882b8(0x304, 1);
        return;
    }
    *(u8 *)(q + 0x379) = 0;
    func_ov002_020882b8(0x305, 1);
}

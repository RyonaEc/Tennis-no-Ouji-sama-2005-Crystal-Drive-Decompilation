#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 *func_ov002_020848b8(void);
extern void func_ov002_02088be0(void);

void func_ov002_02088b0c(void)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *a = p + 0x4bc4;
    u8 *b = p + 0x1e6c;
    u8 *r;
    s8 st;

    r = func_ov002_020848b8();
    if (*(s32 *)(a + 0x364) == 0 && *(s8 *)(a + 0x379) == -1 &&
        *(u8 *)(b + 0x630) != 0 && *(u8 *)(b + 0x632) != 0)
        func_ov002_02088be0();
    st = *(s8 *)(a + 0x379);
    if (st != 0 && st != 1)
        return;
    if (*(u8 *)(a + 0x37a) < 0x10) {
        u8 *c = a + 0x37a;

        *c = *c + 1;
    }
    *(s16 *)(r + 0xa) = *(u8 *)(a + 0x37a);
    if (*(s8 *)(a + 0x379) == 0)
        *(s16 *)(r + 6) = 0xf3;
    else
        *(s16 *)(r + 6) = 0xb3;
}
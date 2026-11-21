#include "types.h"

extern u8 *data_ov005_020c9740;
extern u8 data_ov005_020790a0;
extern void func_ov005_02065c98(u32);
extern void func_02025c94(void *, u32);
extern void func_02023e58(void *, u32, u32);

u32 func_ov005_02064040(void)
{
    u8 *b;
    u8 *p;

    b = data_ov005_020c9740;
    p = b + 0x4300;
    if (*(u16 *)(p + 0xa6) < 0x10) {
        *(u16 *)(b + 0x36) = 2;
        func_ov005_02065c98(0);
        func_02025c94(&data_ov005_020790a0, 0x200);
        func_02023e58(&data_ov005_020790a0, 0xbb80, 0x200);
    } else {
        *(u16 *)(b + 0x4332) = 0;
        *(u16 *)(b + 0x4330) = 0x47;
        *(u16 *)(b + 0x4388) = 0;
    }
    (*(u16 *)(b + 0x43a6))++;
    return 1;
}
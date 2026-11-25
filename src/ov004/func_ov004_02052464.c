#include "types.h"

extern void func_02021af0(u8 *a0, u16 a1, s32 a2);

void func_ov004_02052464(u8 *p, u16 a1, u16 a2, u16 a3)
{
    *(u16 *)(p + 8) = a1;
    if (*(u32 *)p != 0)
        *(u16 *)(p + 8) = *(s16 *)(p + 4);
    *(u16 *)(p + 0xa) = a2;
    *(u16 *)(p + 0xc) = a3;
    *(u16 *)(p + 0xe) = 0;
    *(u16 *)(p + 4) = *(s16 *)(p + 8);
    *(u32 *)p = 1;
    *(u32 *)(p + 0x18) = 0;
    switch (*(s16 *)(p + 6)) {
    case 0:
        func_02021af0((u8 *)0x04000050, 0x3f, *(s16 *)(p + 4));
        break;
    case 1:
        func_02021af0((u8 *)0x04001050, 0x3f, *(s16 *)(p + 4));
        break;
    }
}
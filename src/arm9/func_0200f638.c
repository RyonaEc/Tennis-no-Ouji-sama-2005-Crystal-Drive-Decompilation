#include "types.h"

typedef struct {
    u8 *p0;
    u8 *p4;
    u8 *p8;
    u8 *pc;
    u8 *p10;
} S;

extern volatile S data_02042a68;
extern u8 data_0203cb6c[];
extern u8 data_0203cb6e[];

void func_0200f638(u32 a0, u16 *a1)
{
    s32 i;
    s32 e;

    *a1 = 0;
    i = *(u16 *)(data_0203cb6c + a0 * 0x14);
    e = i + *(u16 *)(data_0203cb6e + a0 * 0x14);
    if (i >= e) return;
    do {
        if (*(u8 *)(*(u32 *)(data_02042a68.p10 + 0x154) + i + 0xcc) != 0) {
            u16 c = *a1;

            *a1 = c + 1;
            *(u16 *)((u8 *)a1 + (c << 1) + 2) = (u16)i;
        }
        i = (u16)(i + 1);
    } while (i < e);
}
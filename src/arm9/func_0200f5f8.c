#include "types.h"

typedef struct {
    u8 *p0;
    u8 *p4;
    u8 *p8;
    u8 *pc;
    u8 *p10;
    u8 *p14;
} S;

extern volatile S data_02042a68;

s32 func_0200f5f8(void)
{
    s32 i;
    s32 n = 0;
    u8 *q = *(u8 **)(data_02042a68.p10 + 0x154);

    for (i = 0; i < 0x13d; i++) {
        if (q[0xcc] != 0) n++;
        q++;
    }
    return n;
}
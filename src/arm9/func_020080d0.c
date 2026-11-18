#include "types.h"

extern u8 data_02042a68[];

typedef struct { u8 b[8]; } Blk;

void func_020080d0(s32 a0, u8 *a1)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10) + a0 * 8 + 0xd0;

    *(Blk *)a1 = *(Blk *)p;
}

#include "types.h"

extern u8 data_02042a68[];

typedef struct { u8 b[8]; } Blk;

void func_0200e8f4(s32 a0, u8 *a1)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10) + a0 * 8 + 0xd0;

    *(Blk *)p = *(Blk *)a1;
}

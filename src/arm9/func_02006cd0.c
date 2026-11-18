#include "types.h"

extern u8 data_02042a68[];

typedef struct { u8 b[16]; } Blk;

void func_02006cd0(u8 *a0)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10) + 0x12c;

    *(Blk *)a0 = *(Blk *)p;
}

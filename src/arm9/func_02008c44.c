#include "types.h"

typedef struct { u32 b:1; } BF;
extern u8 data_02042a68[];
u32 func_02008c44(void)
{
    return ((BF *)(*(u8 **)(data_02042a68 + 0x10)))->b;
}
